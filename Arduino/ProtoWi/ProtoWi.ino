#include "ProtobuffSerial.h"

ProtobuffSerial serialComm;          // - Initialize an instance of the protbuff serial class to do communication
unsigned long cycleTimeMillis = 100;   // - Loop rate in milliseconds.
float desiredHeading = 0.0;
float desiredDistance = 0.0;

void setup(){
  serialComm.InitHw();
  serialComm.Telemetry.MeasuredHeading = 0.0;
  serialComm.Telemetry.MeasuredDistance = 0.0;
}

void loop(){
  serialComm.RunComm();
  // - Simulates acting on a received command in a way that effects
  //   the telemetry sent back to the Arduino.
  performControl();
  delay(cycleTimeMillis); // Delay for the control loop period. Milliseconds
}

void performControl(){
  if (serialComm.Commands.has_WayPointCmd){
    Serial.println("New WayPoint Command Received ... ");
    desiredHeading = serialComm.Commands.WayPointCmd.Heading;
    desiredDistance = serialComm.Commands.WayPointCmd.Distance;
    Serial.print("WayPoint name : ");
    Serial.println(serialComm.Commands.WayPointCmd.Name);
    Serial.print("Waypoint heading : ");
    Serial.println(desiredHeading);
    Serial.print("Waypoint distance : ");
    Serial.println(desiredDistance);
    /// - Clear the way point command... it has been processed.
    serialComm.Commands.has_WayPointCmd = false;
  }
  /// - Send back a measured heading and distance that is a fraction
  ///   of the commanded distance.
  serialComm.Telemetry.MeasuredHeading = desiredHeading*0.75;
  serialComm.Telemetry.MeasuredDistance = desiredDistance*0.75;
}

