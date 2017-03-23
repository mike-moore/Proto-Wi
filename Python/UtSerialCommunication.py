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
        # Populate all required fields
        cmd_packet.NormalizedVoltage = 6.0
        cmd_packet.SecondCommand = 10.0
        self.testArticle.commandArduino(cmd_packet)
        sleep(1)
        cmd_packet.NormalizedVoltage = -6.0
        self.testArticle.commandArduino(cmd_packet)
        sleep(1)


if __name__ == '__main__':
    # Run the unit-tests
    suite = unittest.TestLoader().loadTestsFromTestCase(UtSerialCommunication)
    unittest.TextTestRunner(verbosity=2).run(suite)
