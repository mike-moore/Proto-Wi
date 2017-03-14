import serial, time, threading
from collections import deque
import comm_packet_pb2

class SerialCommunication(object):
	def __init__(self, portName):
		self.serialPort = serial.Serial(port=portName, baudrate=9600, rtscts=True,dsrdtr=True)
		self.readTelemetryThread = threading.Thread(target=self.readTelemetry) 
		self.readTelemetryThread.daemon = True
		self.active = False
		self.tlmQueue = deque(maxlen=10)
		if self.serialPort.isOpen():
			print 'Serial communication running on port : ' + portName

	def run(self):
		self.active = True
		self.readTelemetryThread.start()

	def stop(self):
		self.active = False

	def getTelemetry(self):
		try:
			return self.tlmQueue.popleft()
		except IndexError:
			print 'No telemetry data available. Is your connection active?'

	def readTelemetry(self):
		while self.active:
			bytes_rcvd = self.readRawBytes()
			if bytes_rcvd:
				self.unpackTelemetry(bytes_rcvd)
			time.sleep(0.1)

	def sendCommand(self, cmd):
		self.serialPort.write(cmd)

	def commandArduino(self):
		print 'Sending command ... '
		cmd_packet = comm_packet_pb2.CommandPacket()
		# Populate all required fields
		cmd_packet.NormalizedVoltage = 6.0
		cmd_packet.SecondCommand = 10.0
		# Send down the serialized command
		self.sendCommand(cmd_packet.SerializeToString())

	def readRawBytes(self):
		bytes_read = ''
		while self.serialPort.inWaiting() > 0:
			bytes_read += self.serialPort.read(1)
		return bytes_read

	def unpackTelemetry(self, raw_bytes):
		wb_tlm = comm_packet_pb2.TelemetryPacket()
		wb_tlm.ParseFromString(raw_bytes)
		self.tlmQueue.appendleft(wb_tlm)


