#!/usr/bin/python

import unittest
from time import sleep
# Need a serial communication component for this test
from SerialCommunication import SerialCommunication

class UtSerialCommunication(unittest.TestCase):

    def setUp(self):
        self.testArticle = SerialCommunication("/dev/ttyUSB0")

    def tearDown(self):
        self.testArticle.stop()

    def test_commandArduino(self):
        self.testArticle.commandArduino()


if __name__ == '__main__':
    # Run the unit-tests
    suite = unittest.TestLoader().loadTestsFromTestCase(UtSerialCommunication)
    unittest.TextTestRunner(verbosity=2).run(suite)
