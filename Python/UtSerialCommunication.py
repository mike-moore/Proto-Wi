#!/usr/bin/python

import unittest
from time import sleep
# Need a serial communication component for this test
from SerialCommunication import SerialCommunication
import comm_packet_pb2

class UtSerialCommunication(unittest.TestCase):

    def setUp(self):
        self.testArticle = SerialCommunication("/dev/ttyUSB0")
        self.testArticle.run()

    def tearDown(self):
        self.testArticle.stop()

    def test_commandArduino(self):
        cmd_packet = comm_packet_pb2.CommandPacket()
        # print "Sending empty command packet"
        # self.testArticle.commandArduino(cmd_packet)
        # sleep(1)
        # test sending a way point
#        cmd_packet.WayPointCmd = comm_packet_pb2.WayPoint()
        packets_sent = 0
        while packets_sent < 10:
	        print "Sending way point command"
	        cmd_packet.WayPointCmd.Heading = 45.0
	        cmd_packet.WayPointCmd.Distance = 2.0
	        cmd_packet.WayPointCmd.Name = "Alpha"
	        self.testArticle.commandArduino(cmd_packet)
	        sleep(2)
	        packets_sent += 1


if __name__ == '__main__':
    # Run the unit-tests
    suite = unittest.TestLoader().loadTestsFromTestCase(UtSerialCommunication)
    unittest.TextTestRunner(verbosity=2).run(suite)
