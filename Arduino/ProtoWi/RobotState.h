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
#ifndef ROBOTSTATE_H
#define ROBOTSTATE_H

#include "comm_packet.pb.h"

#define MaxRouteSize 15

///////////////////////////////////////////////////////////////
/// @class RobotState
/// @ingroup WheelBot
/// @brief TODO
///////////////////////////////////////////////////////////////
class RobotState { 
 public:
  RobotState(){};
  ~RobotState(){};

  /// - Sensor readings
  float SensedHeading;
  float SensedDistance;

  /// - Robot guidance
  WayPoint ActiveRoute[MaxRouteSize];
  uint_least8_t NumWayPoints = 0;
  inline void addWayPoint(const WayPoint& way_point);

  /// - Robot control
  float ControlSignal;

  /// - Telemetry
  float ResponseSignal;


 protected:
 	/// - None yet
};
  
inline void RobotState::addWayPoint(const WayPoint& way_point){
  	if (NumWayPoints >= MaxRouteSize){ return ; }
    ActiveRoute[NumWayPoints] = way_point;
    NumWayPoints++;
};

#endif
