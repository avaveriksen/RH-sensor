import os
import time
import csv
import serial
from serial.tools.list_ports import comports

import threading


class SHT45:
    def __init__(self):

        # Data variables
        self.sensors = ['1', '2', '3', '4']
        self.row = [0] * 9  # [Time, T1, T2, T3, T4, RH1, RH2...]
        self.data = [[0, 0, 0, 0, 0, 0, 0, 0, 0]]
        self.time_zero = 0
        self.time_now = 0
        self.logging = True

        # Task management
        self.threading = False

        # Serial variables
        self.baud = 38400
        self.port = '/dev/ttyACM0'  # Adjust as needed
        self.msg = ""
        self.com_ports = []
        self.connected = False

    def add_samples(self):
        self.time_now = time.time()
        self.row[0] = round(self.time_now - self.time_zero, 2)
        # with self.lock:
        self.data.append(self.row.copy())
        print(self.row)
        if self.logging:
            self.save_to_csv()

    def filename_generator(self):
        now = time.localtime(self.time_zero)
        self.filename = time.strftime("%Y%m%d%H%M.csv", now)

    def save_to_csv(self):
        path = os.getcwd() + '/' + self.filename
        with open(path, 'a', newline='') as f:
            data_writer = csv.writer(f)
            data_writer.writerow(self.row)

    def establish_connection(self):
        self.filename_generator()
        try:
            self.ser_conn = serial.Serial(self.port, self.baud, timeout=0.5)
            print("Serial port opened.")
            self.connected = True
        except Exception as e:
            try:
                self.port = "/dev/ttyACM1"
                self.ser_conn = serial.Serial(self.port, self.baud, timeout=0.5)
                print("Serial port opened.")
                self.connected = True
            except Exception as e:
                print("Error opening serial port:", e)
                return

        self.thread1 = threading.Thread(target=self.listen, daemon=True)
        self.thread1.start()

    def decode_message(self):
        msg_fields = self.msg.strip().split("#")
        if len(msg_fields) < 4 or msg_fields[1] != 'D!':
            return
        try:
            sensor = int(msg_fields[2])
            self.row[sensor] = float(msg_fields[3])
            self.row[sensor + 4] = float(msg_fields[4])
        except Exception as e:
            print("Decode error:", e)

    def listen(self):
        if not self.threading:
            self.ser_conn.reset_input_buffer()
            self.ser_conn.reset_output_buffer()
            self.threading = True
        while self.threading:
            try:
                self.msg = self.ser_conn.readline().decode('utf-8')
                #print(self.msg)
                self.decode_message()
                self.add_samples()
            except Exception as e:
                print("Serial read error:", e)
                continue

    def get_ports_list(self):
        ports = comports()
        com_list = [com[0] for com in ports]
        self.com_ports = [port for port in com_list if ('/dev/ttyA' in port or 'COM' in port)]