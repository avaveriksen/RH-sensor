import threading
import serial.tools.list_ports  # pip install pyserial
# Secure the UART serial communication with MCU
import time


class SerialCtrl():
    def __init__(self):
        '''
        Initializing the main varialbles for the serial data
        '''
        self.sync_cnt = 200

    def getCOMList(self):
        '''
        Method that get the lost of available coms in the system
        '''
        ports = serial.tools.list_ports.comports()
        self.com_list = [com[0] for com in ports]
        self.com_list.insert(0, "-")

    def SerialOpen(self, ComGUI):
        '''
        Method to setup the serial connection and make sure to go for the next only 
        if the connection is done properly
        '''

        try:
            self.ser.is_open # Check if there is a serial connection already
        except:
            # If not, try to establish connection with BAUD and PORT from drop-down menus
            PORT = ComGUI.clicked_com.get()
            BAUD = ComGUI.clicked_bd.get()
            self.ser = serial.Serial()
            self.ser.baudrate = BAUD
            self.ser.port = PORT
            self.ser.timeout = 0.1

        try:
            if self.ser.is_open:
                # Check if there is a serial connection, set status flag accordingly
                print("Already Open")
                self.ser.status = True
            else:
                PORT = ComGUI.clicked_com.get()
                BAUD = ComGUI.clicked_bd.get()
                self.ser = serial.Serial()
                self.ser.baudrate = BAUD
                self.ser.port = PORT
                self.ser.timeout = 1
                self.ser.open()
                self.ser.status = True
        except:
            # Failed to establish connection
            self.ser.status = False

    def SerialClose(self, ComGUI):
        '''
        Method used to close the UART communication
        '''
        try:
            self.ser.is_open
            self.ser.close()
            self.ser.status = False
        except:
            self.ser.status = False

    def SerialStop(self, gui):
        # Send message to µController to stop data stream
        self.ser.write(gui.data.StopStream.encode())

    def SerialSync(self, gui):
        # Method to be run in thread when connecting to the µController
        self.threading = True   
        cnt = 0
        while self.threading:
            try:
                self.ser.write(gui.data.sync.encode())      # Transmit message to establish contact with µC
                gui.conn.sync_status["text"] = "..Sync.."   # Gui graphics
                gui.conn.sync_status["fg"] = "orange"
                gui.data.RowMsg = self.ser.readline()       # Wait for incoming message
                gui.data.DecodeMsg()                        # Decode to gui.data.msg
                if gui.data.sync_ok in gui.data.msg[0]:
                    self.ser.write(gui.data.scan.encode())  # command µC to scan i2c for sensors
                    gui.data.RowMsg = self.ser.readline()   # read serial input buffer
                    gui.data.DecodeMsg()                    # Extract sensor information                

                    if int(int(gui.data.msg[1])) > 0:
                        gui.conn.btn_start_stream["state"] = "active"   # Activate buttons
                        gui.conn.btn_add_chart["state"] = "active"
                        gui.conn.btn_kill_chart["state"] = "active"
                        gui.conn.save_check["state"] = "active"
                        gui.conn.sync_status["text"] = "OK"
                        gui.conn.sync_status["fg"] = "green"
                        gui.conn.ch_status["text"] = str(gui.data.n_sensors) # Display number of sensors
                        gui.conn.ch_status["fg"] = "green"
                        gui.data.SynchChannel = gui.data.n_sensors * 2  # Set num of sensor channels, 2 channels pr. sensor
                        gui.data.GenChannels()              # Generate channel options for GUI
                        gui.data.buildYdata()               # Generate storage arrays for channels
                        gui.data.FileNameFunc()             # Generate filename for logged data
                        self.threading = False              # Stop sync while loop
                        break
                if self.threading == False:
                    break
            except Exception as e:
                print(e)
            cnt += 1
            if self.threading == False:
                break
            if cnt > self.sync_cnt:
                cnt = 0
                gui.conn.sync_status["text"] = "failed"
                gui.conn.sync_status["fg"] = "red"
                time.sleep(0.5)
                if self.threading == False:
                    break

    def SerialDataStream(self, gui):
        # method to run as a thread. 
        self.threading = True
        cnt = 0
        while self.threading:
        # send command to start streaming, receive ack message
            try:
                # get data
                self.ser.write(gui.data.StartStream.encode())   # transmit message to µC to start data stream
                gui.data.RowMsg = self.ser.readline()           # read latest message in serial input buffer
                gui.data.DecodeMsg()                            # decode and check if µC streams data
                if gui.data.StreamData:
                    gui.data.SetRefTime()
                    break # Enter next loop
            except Exception as e:
                print(e)
        gui.UpdateChart()
        while self.threading:
        # maintain streaming process
            try:
                gui.data.RowMsg = self.ser.readline()
                gui.data.DecodeMsg()    # decode data message, store in arrays
                if gui.data.StreamData:
                    gui.data.AdjustData()

                    if gui.save:
                        gui.data.SaveData(gui)
                        '''
                        t1 = threading.Thread(
                            target=gui.data.SaveData, args=(gui,), daemon=True)
                        t1.start()
                        '''
            except Exception as e:
                print(e)
