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

	def commandArduino(self, cmd):
		if (isinstance(cmd, comm_packet_pb2.CommandPacket)):
			# Send down the serialized command
			self.sendCommand(cmd.SerializeToString())
		else:
			raise TypeError


	def readRawBytes(self):
		bytes_read = ''
		while self.serialPort.inWaiting() > 0:
			bytes_read += self.serialPort.read(1)
		return bytes_read

	def unpackTelemetry(self, raw_bytes):
		wb_tlm = comm_packet_pb2.TelemetryPacket()
		wb_tlm.ParseFromString(raw_bytes)
		print "Received Data"
		print "Position : " + str(wb_tlm.Position)
		print "Velocity : " + str(wb_tlm.Velocity)
		print "New Telemetry : " + str(wb_tlm.NewTelemetry)
		self.tlmQueue.appendleft(wb_tlm)


