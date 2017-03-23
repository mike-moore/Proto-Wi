import serial, time, threading
from collections import deque
import comm_packet_pb2
import logging
from google.protobuf.message import EncodeError

class SerialCommunication(object):
    def __init__(self, portName):
        self.serialPort = serial.Serial(
            port=portName, baudrate=9600, rtscts=True, dsrdtr=True)
        self.resetPacketCounters()
        if self.serialPort.isOpen():
            logging.info('Serial communication running on port : ' + portName)
            self.serialPort.flushInput()
            self.serialPort.flushOutput()

    def sendCommand(self, cmd):
        logging.debug(":".join("{:02x}".format(ord(c)) for c in cmd))
        self.serialPort.write(cmd)

    def commandArduino(self, cmd):
        if (isinstance(cmd, comm_packet_pb2.CommandPacket)):
            # Send down the serialized command
            try:
                self.sendCommand(cmd.SerializeToString())
            except EncodeError:
            	logging.error("Failed to encode command packet. Are all required fields set?")
            	raise IOError

            # Give the Arduino time to respond.
            time.sleep(1.0)

            # Unpack the received Arduino packet.
            try:
                response = self.readTelemetry()
                self.NumReceivedPackets += 1
                return response
            except IOError:
                self.NumFailedPackets += 1
                return None
        else:
            raise TypeError

    def readTelemetry(self):
        bytes_rcvd = self.readRawBytes()
        if bytes_rcvd:
            return self.unpackTelemetry(bytes_rcvd)
        else:
            raise IOError

    def readRawBytes(self):
        bytes_read = ''
        while self.serialPort.inWaiting() > 0:
            bytes_read += self.serialPort.read(1)
        return bytes_read

    def unpackTelemetry(self, raw_bytes):
        wb_tlm = comm_packet_pb2.TelemetryPacket()
        logging.debug("Bytes to be unpacked :")
        logging.debug(":".join("{:02x}".format(ord(c)) for c in raw_bytes))
        try:
            wb_tlm.ParseFromString(raw_bytes)
        except Exception:
            raise IOError
        return wb_tlm

    def resetPacketCounters(self):
        self.NumFailedPackets = 0
        self.NumReceivedPackets = 0
