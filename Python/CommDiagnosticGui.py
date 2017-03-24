#!/usr/bin/python
from PyQt4 import QtCore, QtGui, uic
from CommQThread import *
from ResponsePlots import *
import sys
import glob
import serial


qtCreatorFile = "DiagnosticGui.ui"
Ui_Comm_Window, QtBaseClass = uic.loadUiType(qtCreatorFile)

class CommDiagnosticGui(QtGui.QMainWindow, Ui_Comm_Window):

    def __init__(self):
        QtGui.QMainWindow.__init__(self)
        Ui_Comm_Window.__init__(self)
        self.setupUi()

    def setupUi(self):
        super(CommDiagnosticGui, self).setupUi(self)

        #Combo box should list available serial ports
        self.PortcomboBox.addItems(self.serialPorts())

        #Connect and Disconnect Buttons with Serial ports
        self.Connect_Btn.clicked.connect(self.connectToArd)
        self.Disconnect_Btn.clicked.connect(self.disconnectFromArd)

        # Add amplitude slider value changed event
        self.AmplitudeSlider.valueChanged.connect(self.amplitudeChange)

        #Connect plots to the plot layout
        self.responsePlot = ResponsePlotCanvas(None, width=5, height=4, dpi=100)
        self.Lo_Plots.addWidget(self.responsePlot)

    def serialPorts(self):
        if sys.platform.startswith('win'):
            ports = ['COM%s' % (i + 1) for i in range(256)]
        elif sys.platform.startswith('linux') or sys.platform.startswith('cygwin'):
            # this excludes your current terminal "/dev/tty"
            ports = glob.glob('/dev/tty[A-Za-z]*')
        elif sys.platform.startswith('darwin'):
            ports = glob.glob('/dev/tty.*')
        else:
            raise EnvironmentError('Unsupported platform')

        result = []
        for port in ports:
            try:
                s = serial.Serial(port)
                s.close()
                result.append(port)
            except (OSError, serial.SerialException):
                pass
        return result

    def connectToArd(self):
        #start comm thread
        self.SerialCommThread = CommQThread(str(self.PortcomboBox.currentText()))

        # signal connection for state update callback
        self.connect(self.SerialCommThread, SIGNAL("new_data_received(PyQt_PyObject)"), self.stateUpdate)

        self.SerialCommThread.connectSlot(self)

        # Connect live plots to the serial comm thread. Needed for data updates.
        self.responsePlot.connectSlot(self.SerialCommThread)

        self.SerialCommThread.start()

    def disconnectFromArd(self):
        if (self.SerialCommThread):
            self.SerialCommThread.terminate()

    def amplitudeChange(self):
        scaledAmplitude = self.scale(self.AmplitudeSlider.value(), (0.0,100.0), (-1.0,+1.0))
        cmd_packet = comm_packet_pb2.CommandPacket()
        control_signal_cmd = cmd_packet.RoverCmds.add()
        control_signal_cmd.Id = CTRL_ACTIVE
        control_signal_cmd.Value = scaledAmplitude
        self.AmplitudeValue.setText("%.2f"%scaledAmplitude)
        self.emit(SIGNAL('send_cmds(PyQt_PyObject)'), cmd_packet)

    def stateUpdate(self, response):
        # called when new data arrives over the serial port
        return

    def scale(self, val, src, dst):
        """
        Scale the given value from the scale of src to the scale of dst.
        """
        return ((val - src[0]) / (src[1]-src[0])) * (dst[1]-dst[0]) + dst[0]

if __name__ == "__main__":
    app = QtGui.QApplication(sys.argv)
    window = CommDiagnosticGui()
    window.show()
    sys.exit(app.exec_())
