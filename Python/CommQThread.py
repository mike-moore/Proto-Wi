from PyQt4.QtCore import QThread, QObject, SIGNAL
from SerialCommunication import SerialCommunication
import numpy as np
from CmdResponseDefinitions import *
import comm_packet_pb2


class CommQThread(QThread):
    def __init__(self, serialPort):
        QThread.__init__(self)
        self.serialPort = serialPort
        self.serialComm = SerialCommunication(serialPort)
        self.cmd = comm_packet_pb2.CommandPacket()
        control_signal_cmd = self.cmd.RoverCmds.add()
        control_signal_cmd.Id = CTRL_ACTIVE
        control_signal_cmd.Value = 0.0

    def __del__(self):
        self.wait()

    def connectSlot(self, uiThread):
        self.connect(uiThread, SIGNAL("send_cmds(PyQt_PyObject)"), self.processCmds)

    def run(self):
        while True:
            # Repeatedly send our command packet and emit a signal containing the response
            response = self.serialComm.commandArduino(self.cmd)
            if response:
                self.emit(SIGNAL('new_data_received(PyQt_PyObject)'), response)

    def processCmds(self, cmd_packet):
        self.cmd = cmd_packet
