import os
import time
import csv
import serial
from serial.tools.list_ports import comports

import threading


class SHT45:
    def __init__(self):

        # Data variables
        self.sensors = [""] * 4
        self.n_devices = 0
        self.row = [0] * 9  # [Time, T1, T2, T3, T4, RH1, RH2...]
        self.data = []
        self.time_zero = 0
        self.time_now = 0
        self.logging = True

        # Task management
        self.threading = False
        self.streaming = False

        # Serial variables
        self.baud = 38400
        self.port = '/dev/ttyACM0'  # Adjust as needed
        self.msg = ""
        self.com_ports = []
        self.connected = False

        # csv file
        self.path = ""

    def add_samples(self):
        self.time_now = time.time()
        self.row[0] = round(self.time_now - self.time_zero, 2)
        # with self.lock:
        self.data.append(self.row.copy())
        #print(self.row)
        if self.logging:
            self.save_to_csv()

    def filename_generator(self):
        now = time.localtime(self.time_zero)
        self.filename = time.strftime("%Y-%m-%d-%H%M.csv", now)

    def save_to_csv(self):
        if self.path == "":
            self.path = os.getcwd() + '/' + self.filename
            csv_title = ["RH and T log", time.ctime()]
            csv_header = ["Time (s)", "RH1", "RH2", "RH3", "RH4", "T1", "T2", "T3", "T4"]
            with open(self.path, 'a', newline='') as f:
                data_writer = csv.writer(f)
                data_writer.writerow(csv_title)
                data_writer.writerow(csv_header)
                data_writer.writerow(self.row)
        else:
            with open(self.path, 'a', newline='') as f:
                data_writer = csv.writer(f)
                data_writer.writerow(self.row)


    def establish_connection(self):
        self.filename_generator()
        try:
            self.ser_conn = serial.Serial(self.port, self.baud, timeout=3)
            print("Serial port opened.")
        except Exception as e:
            print("Error opening serial port:", e)
            return

        try:
            self.ser_conn.reset_input_buffer()
            self.ser_conn.reset_output_buffer()
            self.ser_conn.write(b'#?#\n')
            self.msg = self.ser_conn.readline().decode('utf-8')
            self.decode_message()
        except Exception as e:
            print("Error: No ACK from transmitter", e)

        self.ser_conn.write(b'#s#\n')
        time.sleep(0.1)
        while self.ser_conn.in_waiting > 0:
            self.msg = self.ser_conn.readline().decode('utf-8')
            self.decode_message()

    def close_connection(self):
        try:
            self.ser_conn.close()
            self.connected = False
            print("Serial port at " + self.port +" closed.")
        except Exception as e:
            print("Error closing serial port at " + self.port, e)

    def start_stream(self):
        self.ser_conn.write(b'#A#\n')   # command to start streaming
        self.msg = self.ser_conn.readline().decode('utf-8')
        self.decode_message()
        if self.streaming:
            self.ser_conn.reset_input_buffer()
            self.ser_conn.reset_output_buffer()
            self.threading = True
            self.thread1 = threading.Thread(target=self.listen, daemon=True)
            self.thread1.start()

    def stop_stream(self):
        print("Stopping stream")
        self.threading = False
        self.ser_conn.reset_input_buffer()
        self.ser_conn.reset_output_buffer()
        self.ser_conn.write(b'#S#\n')   # Command to stop stream
        self.msg = self.ser_conn.readline().decode('utf-8')
        print("self.msg:" + self.msg)
        self.decode_message()
        if not self.streaming:
            print("stop_stream(): self.threading = False")
            self.threading = False

    def decode_message(self):
        msg_fields = self.msg.strip().split("#")
        #print(msg_fields)
        if (msg_fields[1] == '!!' or msg_fields[1] == '!'):
            self.connected = True
        elif msg_fields[1] == 'A!':
            self.streaming = True
        elif msg_fields[1] == 'S!':
            self.streaming = False
        elif msg_fields[1] == 's!': 
            channel = msg_fields[2]
            self.sensors[int(channel) - 1] = msg_fields[3]
        elif msg_fields[1] == 'D':
            try:
                sensor = int(msg_fields[2])
                self.row[sensor] = float(msg_fields[3])
                self.row[sensor + 4] = float(msg_fields[4])
            except Exception as e:
                print("Decode error:", e)
        elif msg_fields[1] == 'D!':
            self.add_samples()
        elif len(msg_fields) < 4 or msg_fields[1] != 'D!':
            return

    def listen(self):
        while self.threading:
            while self.ser_conn.in_waiting > 0:
                try:
                    self.msg = self.ser_conn.readline().decode('utf-8')
                    #print(self.msg)
                    self.decode_message()
                    time.sleep(0.01)
                except Exception as e:
                    print("Serial read error:", e)
                    continue

    def get_ports_list(self):
        ports = comports()
        com_list = [com[0] for com in ports]
        self.com_ports = [port for port in com_list if ('/dev/ttyA' in port or 'COM' in port)]