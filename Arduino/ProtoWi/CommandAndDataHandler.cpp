#include "CommandAndDataHandler.h"

CommandAndDataHandler::CommandAndDataHandler(CommandPacket& commands, TelemetryPacket tlm, RobotState state)
 :
 Commands(commands),
 Telemetry(tlm),
 State(state)
{}

CommandAndDataHandler::~CommandAndDataHandler() {}

void CommandAndDataHandler::ProcessCmds() {
    Serial.println("Processing New Command Received ... ");
    /// - Iterate over and process any rover commands that were sent
    for (uint_least8_t indx = 0; indx < Commands.RoverCmds_count; indx++){
        ProcessRoverCmd(Commands.RoverCmds[indx]);
    }
    /// - Process the way point command if it was sent (must have name to be 
    ///   a valid way-point so we just check for that)
    if (strcmp(Commands.WayPointCmd.Name, "INVALID")){
        ProcessWayPointCmd(Commands.WayPointCmd);
    }
}

void CommandAndDataHandler::LoadTelemetry() {
    /// - Load up all the required telemetry based on the robot's state.
    Telemetry.MeasuredHeading = State.SensedHeading;
    Telemetry.MeasuredDistance = State.SensedDistance;
    /// - Load the conditional telemetry. Data that has to be requested
    ///   in order to be sent.
    LoadRoverStatus();
}

void CommandAndDataHandler::ProcessRoverCmd(const IdValuePairFloat & rover_cmd) {
    Serial.print("Rover Command Received : ");
    Serial.println(rover_cmd.Id);
    Serial.print("Rover Command Value : ");
    Serial.println(rover_cmd.Value);
    if (rover_cmd.Id == 0xA501){
        SendResponseSignal = true;
        State.ControlSignal = rover_cmd.Value;
    }
}

void CommandAndDataHandler::ProcessWayPointCmd(const WayPoint & way_point_cmd) {
    /// - Add the way point to robot's state registry. This will make it
    ///   available for the guidance module.
    State.addWayPoint(way_point_cmd);
    /// - Debug message.
    Serial.print("Adding WayPoint ");
    Serial.print(way_point_cmd.Name);
    Serial.println(" to route.");
    Serial.print("WayPoint heading : ");
    Serial.println(way_point_cmd.Heading);
    Serial.print("WayPoint distance : ");
    Serial.println(way_point_cmd.Distance);
}

void CommandAndDataHandler::PackResponseSignal(uint_least8_t status_indx) {
    Telemetry.RoverStatus[status_indx].Id = 0xB501;
    Telemetry.RoverStatus[status_indx].Value = State.ResponseSignal;
}

void CommandAndDataHandler::LoadRoverStatus() {
    uint_least8_t status_indx = 0;
    if(SendResponseSignal){
        PackResponseSignal(status_indx);
        status_indx++;
    }
}

