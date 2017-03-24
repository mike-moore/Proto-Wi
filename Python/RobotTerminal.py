from SerialCommunication import SerialCommunication
from CmdResponseDefinitions import *
from cmd import Cmd
from time import sleep

class RobotTerminal(Cmd):

    def __init__(self):
        self.portName = "/dev/ttyUSB0"

    def do_set_port(self, args):
        self.portName = args

    def do_connect(self, args):
        """Connects to the robot. Required before sending commands."""
        self.testArticle = SerialCommunication(self.portName)

    def do_exit(self, args):
        """Exits the terminal"""
        raise SystemExit

    def do_send_waypoint(self, args):
        """Prompts the user to enter a way-point and then sends it to the Arduino"""

    def do_start_control(self, args):
        """Prompts the user to enter a way-point and then sends it to the Arduino"""

if __name__ == '__main__':
    prompt = RobotTerminal()
    prompt.prompt = '>'
    prompt.cmdloop('Robot Terminal ...')
