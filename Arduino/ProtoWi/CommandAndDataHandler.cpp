#include "CommandAndDataHandler.h"

CommandAndDataHandler::CommandAndDataHandler(CommandPacket& commands, TelemetryPacket& tlm, RobotState& state)
 :
 Commands(commands),
 Telemetry(tlm),
 State(state)
{}

CommandAndDataHandler::~CommandAndDataHandler() {}

void CommandAndDataHandler::ProcessCmds() {
    /// - Clear the count of Rover Status variables to be sent.
    ///   This is determined based on which RoverCmds are
    ///   received. It is incremented in LoadRoverStatus function.
    Telemetry.RoverStatus_count = 0;
    /// - Iterate over and process any rover commands that were sent
    for (uint_least8_t indx = 0; indx < Commands.RoverCmds_count; indx++){
        Serial.println("Processing New Rover Command Received ... ");
        ProcessRoverCmd(Commands.RoverCmds[indx]);
    }
    /// - Process the way point command if it was sent and valid
    ///   Invalid or no way-point sent defaults to 0 length string for its name
    if (strlen(Commands.WayPointCmd.Name)==0){
        /// - Pack the waypoint rejected command
        PackInt(WP_CMD_REJECT);
        return;
    }else{
        ProcessWayPointCmd(Commands.WayPointCmd);
        /// - Pack the waypoint acknowledged command
        PackInt(WP_CMD_ACCEPT);
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

void CommandAndDataHandler::ProcessRoverCmd(IdValuePairFloat & rover_cmd) {
    Serial.print("Rover Command Received : ");
    Serial.println(rover_cmd.Id);
    Serial.print("Rover Command Value : ");
    Serial.println(rover_cmd.Value);
    if (rover_cmd.Id == CTRL_ACTIVE){
        SendResponseSignal = true;
        State.ControlSignal = rover_cmd.Value;
    }
}

void CommandAndDataHandler::ProcessWayPointCmd(WayPoint & way_point_cmd) {
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

void CommandAndDataHandler::PackInt(uint32_t id) {
    Telemetry.RoverStatus[Telemetry.RoverStatus_count].Id = id;
    Telemetry.RoverStatus_count++;
}

void CommandAndDataHandler::PackFloat(uint32_t id, float value) {
    Telemetry.RoverStatus[Telemetry.RoverStatus_count].Id = id;
    Telemetry.RoverStatus[Telemetry.RoverStatus_count].Value = value;
    Telemetry.RoverStatus[Telemetry.RoverStatus_count].has_Value = true;
    Telemetry.RoverStatus_count++;
}

void CommandAndDataHandler::LoadRoverStatus() {
    if(SendResponseSignal){
        PackFloat(RESPONSE_SIGNAL, State.ResponseSignal);
        SendResponseSignal = false;
    }
}

