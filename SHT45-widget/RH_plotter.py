import sys
import time
import threading
import serial
import csv

from matplotlib.backends.backend_qtagg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.figure import Figure
from PyQt6 import QtWidgets
from PyQt6.QtCore import QSize
from SHT45_lib import SHT45
#from plot_demo import demo_data





class ApplicationWindow(QtWidgets.QMainWindow):
    def __init__(self, sensor):
        super().__init__()
        self.sensor = sensor

        # Window
        self.setWindowTitle("SHT45 Widget")

        # Master Layout and master widget
        self.layout_master = QtWidgets.QHBoxLayout()        # Layout to hold nested layouts
        central_widget = QtWidgets.QWidget()

        # nested vertical plots layout
        self.layout_plots = QtWidgets.QVBoxLayout()         # create layout
        self.plotwidget = QtWidgets.QWidget()               # empty widget to hold plots
        self.plotwidget.setMinimumWidth(600)                # size constraints of the widget
        self.plotwidget.setLayout(self.layout_plots)        # assign layout to widget
        self.layout_master.addWidget(self.plotwidget)       # add nested layout to master layout

        # vertical controls layout
        self.layout_controls = QtWidgets.QVBoxLayout()
        self.controls_widget = QtWidgets.QWidget()          # Empty widget to hold nested widgets
        self.controls_widget.setMinimumWidth(150)
        self.controls_widget.setLayout(self.layout_controls)

        self.connection_box =QtWidgets.QGroupBox("Connection")
        self.display_box = QtWidgets.QGroupBox("Display")
        self.log_box = QtWidgets.QGroupBox("Data Logging")

        self.layout_connection = QtWidgets.QVBoxLayout()
        self.com_port_box = QtWidgets.QComboBox()
        self.refresh_com = QtWidgets.QPushButton("Refresh Ports")
        self.connect_button = QtWidgets.QPushButton("Connect")
        self.start_button = QtWidgets.QPushButton("Start")
        self.layout_connection.addWidget(self.com_port_box)
        self.layout_connection.addWidget(self.refresh_com)
        self.layout_connection.addWidget(self.connect_button)
        self.layout_connection.addWidget(self.start_button)
        self.connection_box.setLayout(self.layout_connection)

        self.layout_display = QtWidgets.QVBoxLayout()
        self.checkbox_ch1 = QtWidgets.QCheckBox("Ch1")
        self.checkbox_ch2 = QtWidgets.QCheckBox("Ch2")
        self.checkbox_ch3 = QtWidgets.QCheckBox("Ch3")
        self.checkbox_ch4 = QtWidgets.QCheckBox("Ch4")
        self.layout_display.addWidget(self.checkbox_ch1)
        self.layout_display.addWidget(self.checkbox_ch2)
        self.layout_display.addWidget(self.checkbox_ch3)
        self.layout_display.addWidget(self.checkbox_ch4)
        self.display_box.setLayout(self.layout_display)

        self.layout_log = QtWidgets.QVBoxLayout()
        self.checkbox_log = QtWidgets.QCheckBox("Log Data")
        self.layout_log.addWidget(self.checkbox_log)
        self.log_box.setLayout(self.layout_log)
        self.checkbox_log.setChecked(True)

        self.spacer = QtWidgets.QWidget()

        # Button events
        self.refresh_com.clicked.connect(self.refresh_ports)  # when clicking refresh button
        self.connect_button.clicked.connect(self.connect_button_event)
        self.start_button.clicked.connect(self.start_button_event)
        self.com_port_box.currentTextChanged.connect(self.port_changed)
        self.checkbox_ch1.stateChanged.connect(self.log_state_changed)




        # Add boxes to nested widget
        self.layout_controls.addWidget(self.connection_box)
        self.layout_controls.addWidget(self.display_box)
        self.layout_controls.addWidget(self.log_box)
        self.layout_controls.addWidget(self.spacer)

        self.layout_master.addWidget(self.controls_widget)

        # Place layouts in widget, set central widget

        central_widget.setLayout(self.layout_master)
        self.setCentralWidget(central_widget)

        # Create Plotting Widget
        self.canvas = FigureCanvas(Figure(figsize=(5, 6)))
        self.layout_plots.addWidget(self.canvas)

        self._axRH, self._axT = self.canvas.figure.subplots(2, 1, sharex = True)
        self._axRH.set_title("Relative Humidity")
        self._axT.set_title("Temperature")


        self._axRH.grid()
        self._axT.grid()

        self._axRH.set_ylabel("Relative Humidity (%)")
        self._axT.set_ylabel("Temperature (C)")
        self._axT.set_xlabel("Time (s)")

        self._lineRH, = self._axRH.plot([], [], color='tab:blue')
        self._lineT, = self._axT.plot([], [], color='tab:red')

        self.xdata = []
        self.RHdataCH1 = []
        self.RHdataCH2 = []
        self.RHdataCH3 = []
        self.RHdataCH4 = []
        self.TdataCH1 = []
        self.TdataCH2 = []
        self.TdataCH3 = []
        self.TdataCH4 = []


        self.data_timer = self.canvas.new_timer(250)  # Update every 250ms
        self.data_timer.add_callback(self.update_plot_1)
        self.data_timer.start()

    def _update_plot(self):
        # with self.instrument.lock:
        l = len(self.sensor.data)
        if l > 50:
            self.RHdata1 = [row[1] for row in self.sensor.data[-50:]]
            self.RHdata2 = [row[2] for row in self.sensor.data[-50:]]
            self.Tdata = [row[5] for row in self.sensor.data[-50:]]
            self.xdata = [row[0] for row in self.sensor.data[-50:]]
        else:
            self.RHdata1 = [row[1] for row in self.sensor.data]
            self.Tdata = [row[5] for row in self.sensor.data[-50:]]
            self.xdata = [row[0] for row in self.sensor.data]

        self._lineRH.set_data(self.xdata, [self.RHdata1, self.RHdata2])
        self._lineT.set_data(self.xdata, self.Tdata)
        self._axRH.relim()
        self._axRH.autoscale_view()

        self._axT.relim()
        self._axT.autoscale_view()

        self.canvas.draw_idle()

    def update_plot_1(self):
        l = len(self.sensor.data)
        if l >= 50:
            self.RHdataCH1 = [row[1] for row in self.sensor.data[-50:]]
            self.RHdataCH2 = [row[2] for row in self.sensor.data[-50:]]
            self.RHdataCH3 = [row[3] for row in self.sensor.data[-50:]]
            self.RHdataCH4 = [row[4] for row in self.sensor.data[-50:]]

            self.TdataCH1 = [row[5] for row in self.sensor.data[-50:]]
            self.TdataCH2 = [row[6] for row in self.sensor.data[-50:]]
            self.TdataCH3 = [row[5] for row in self.sensor.data[-50:]]
            self.TdataCH4 = [row[8] for row in self.sensor.data[-50:]]

            self.xdata = [row[0] for row in self.sensor.data[-50:]]
        else:
            self.RHdataCH1 = [row[1] for row in self.sensor.data]
            self.RHdataCH2 = [row[2] for row in self.sensor.data]
            self.RHdataCH3 = [row[3] for row in self.sensor.data]
            self.RHdataCH4 = [row[4] for row in self.sensor.data]
            self.TdataCH1 = [row[5] for row in self.sensor.data]
            self.TdataCH2 = [row[6] for row in self.sensor.data]
            self.TdataCH3 = [row[7] for row in self.sensor.data]
            self.TdataCH4 = [row[8] for row in self.sensor.data]
            self.xdata = [row[0] for row in self.sensor.data]

        self._axRH.clear()
        self._axT.clear()

        if self.checkbox_ch1.isChecked():
            self._axRH.plot(self.xdata, self.RHdataCH1, color='tab:blue')
            self._axT.plot(self.xdata, self.TdataCH1, color='tab:blue')

        if self.checkbox_ch2.isChecked():
            self._axRH.plot(self.xdata, self.RHdataCH2, color='tab:red')
            self._axT.plot(self.xdata, self.TdataCH2, color='tab:red')

        if self.checkbox_ch3.isChecked():
            self._axRH.plot(self.xdata, self.RHdataCH3, color='tab:green')
            self._axT.plot(self.xdata, self.TdataCH3, color='tab:green')

        if self.checkbox_ch4.isChecked():
            self._axRH.plot(self.xdata, self.RHdataCH4, color='tab:orange')
            self._axT.plot(self.xdata, self.TdataCH4, color='tab:orange')

        self.canvas.draw_idle()

    def refresh_ports(self):
        print("Refresh ports...")
        self.sensor.get_ports_list()
        self.com_port_box.clear()
        self.com_port_box.addItems(self.sensor.com_ports)

    def port_changed(self):
        print("Port changed.")
        self.sensor.port = self.com_port_box.currentText()

    def connect_button_event(self):
        if self.connect_button.text()=="Connect":
            self.sensor.establish_connection()
            if self.sensor.connected:
                self.connect_button.setText("Disconnect")
        elif self.connect_button.text()=="Disconnect":
            pass
            # write a function to break connection and stop logging

    def start_button_event(self):
        self.sensor.start_stream()
        if self.sensor.streaming:
            self.start_button.setText("Stop Stream")

    def log_state_changed(self):
        if self.checkbox_log.isChecked():
            self.sensor.logging = True
        else:
            self.sensor.logging = False

if __name__ == "__main__":

    sensor = SHT45()

    #sensor.establish_connection()
    sensor.time_zero = time.time()
    #sensor.data = demo_data().data

    app = QtWidgets.QApplication([])        # the brains of the app, holds the event loop
    window = ApplicationWindow(sensor)      # holds the interface we see
    window.show()
    sys.exit(app.exec())
