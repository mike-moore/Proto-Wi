#!/usr/bin/python
from SerialCommunication import SerialCommunication
from CmdResponseDefinitions import *
import comm_packet_pb2
from cmd import Cmd
from time import sleep
import numpy as np

class RobotTerminal(Cmd):

    def __init__(self):
        Cmd.__init__(self)
        self.portName = "/dev/ttyUSB0"

    def do_set_port(self, args):
        """Set the port name for the Arduino. eg set_port /dev/ttyUSB0"""
        self.portName = args

    def do_connect(self, args):
        """Connects to the robot. Required before sending commands."""
        self.testArticle = SerialCommunication(self.portName)

    def do_exit(self, args):
        """Exits the terminal"""
        raise SystemExit

    def do_send_waypoint(self, args):
        """Prompts the user to enter a way-point and then sends it to the Arduino"""
        self.send_waypoint()
        return

    def do_start_control(self, args):
        """Sends a sine wave control signal to the robot."""
        try:

            print " This command will send a sine wave to the robot. Paramterize the sine wave "
            print "Enter amplitude : "
            amplitude = float(raw_input())
            print "Enter frequency : "
            frequency = float(raw_input())
        except ValueError:
            print "Invalid amplitude or frequency. Both must be floats."
        sine_wave = self.generateSineWave(amplitude, frequency)
        for sample in sine_wave:
            cmd_packet = comm_packet_pb2.CommandPacket()
            control_signal_cmd = cmd_packet.RoverCmds.add()
            control_signal_cmd.Id = CTRL_ACTIVE
            control_signal_cmd.Value = sample
            response = self.testArticle.commandArduino(cmd_packet)
            if response:
                print " Response signal : " + str(response.RoverStatus[1].Value)

    def send_waypoint(self):
        try:

            print "WayPoint Name : "
            way_point_name = raw_input()
            print "Heading : "
            way_point_heading = float(raw_input())
            print "Distance : "
            way_point_distance = float(raw_input())
        except ValueError:
            print "Invalid waypoint. Name must be a string less than 15 characters. Heading and distance must be a float."
        way_point_cmd = comm_packet_pb2.CommandPacket()
        way_point_cmd.WayPointCmd.Name = way_point_name
        way_point_cmd.WayPointCmd.Heading = way_point_heading
        way_point_cmd.WayPointCmd.Distance = way_point_distance
        response = self.testArticle.commandArduino(way_point_cmd)
        if self.isValidWayPoint(response):
            print "WayPoint " + way_point_name + " successfully sent and processed."
        else:
            print "WayPoint " + way_point_name + " command was rejected."

    def isValidWayPoint(self, response):
        if response:
            if response.RoverStatus[0].Id == WP_CMD_ACCEPT:
                return True
        return False

    def generateSineWave(self, amplitude=1.0, frequency=1.0, duration=1.0, stepSize=0.02):
        t = np.arange(0, duration, stepSize)
        sine_wave = amplitude*np.sin((2*np.pi*frequency)*t)
        return sine_wave

if __name__ == '__main__':
    prompt = RobotTerminal()
    prompt.prompt = '>> '
    prompt.cmdloop('Welcome to the Robot Terminal. Type help to see a list of commands.')
