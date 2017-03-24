///////////////////////////////////////////////////////////////
///  This file defines a class that is used to implement
///  the TODO.
///
/// @author
///         $Author: Mike Moore $
///
/// Contact: mike.moore@so.engineering
///
/// Created on: March 23 2017
///
///////////////////////////////////////////////////////////////
#ifndef COMMANDPROCESSOR_H
#define COMMANDPROCESSOR_H

#include <Arduino.h>
#include "comm_packet.pb.h"
#include "RobotState.h"

///////////////////////////////////////////////////////////////
/// @class CommandAndDataHandler
/// @ingroup Communication
/// @brief TODO
///////////////////////////////////////////////////////////////
class CommandAndDataHandler { 
 public:
  CommandAndDataHandler(CommandPacket& commands, TelemetryPacket& tlm, RobotState& state);
  ~CommandAndDataHandler();

  void ProcessCmds();
  void LoadTelemetry();

 protected:
  CommandPacket& Commands;
  TelemetryPacket& Telemetry;
  RobotState& State;
  bool SendResponseSignal;
  void ProcessRoverCmd(IdValuePairFloat & rover_cmd);
  void ProcessWayPointCmd(WayPoint & way_point_cmd);
  void PackResponseSignal(uint_least8_t status_indx);
  void LoadRoverStatus();
};

#endif
