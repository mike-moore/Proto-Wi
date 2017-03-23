///////////////////////////////////////////////////////////////
///  This file defines a class that is used to implement
///  the TODO.
///
/// @author
///         $Author: Mike Moore $
///
/// Contact: mike.moore@so.engineering
///
/// Created on: September 15 2016
///
///////////////////////////////////////////////////////////////
#ifndef PROTOBUFFSERIAL_H
#define PROTOBUFFSERIAL_H

#include <Arduino.h>
#include <SoftwareSerial.h>
#include "pb_encode.h"
#include "pb_decode.h"
#include "comm_packet.pb.h"

#define COMM_MAX_BUFF_SIZE 200

///////////////////////////////////////////////////////////////
/// @class ProtobuffSerial
/// @ingroup Communication
/// @brief TODO
///////////////////////////////////////////////////////////////
class ProtobuffSerial { 
 public:
  ProtobuffSerial();
  ~ProtobuffSerial();

  typedef enum CommStatusCodes
  {
    RX_PACKET_SUCCESS = 5,
    TX_PACKET_SUCCESS = 4,
    TX_PACKET_WAITING = 3,
    RX_PACKET_READY = 2,
    RX_READING_PACKET = 1,
    RX_PACKET_FAIL = -1,
    UNLOAD_FAIL = -2,
    LOAD_FAIL = -3,
  }CommStatusCode;

  void InitHw();
  int Rx();
  int Tx();
  inline bool NewCommandsArrived() {return NewCmdsFlag ; };

  CommandPacket Commands;
  TelemetryPacket Telemetry;

 protected:
  SoftwareSerial mySerial = SoftwareSerial(2,3);
  int ReadPacket();
  void WritePacket();
  bool Encode();
  bool Decode();
  void ClearBuffers();
  void ClearBuffersAndReset();
  uint_least8_t TxBuffer[COMM_MAX_BUFF_SIZE];
  uint_least8_t RxBuffer[COMM_MAX_BUFF_SIZE];
  uint_least8_t RxByteCounter;
  uint_least8_t NumBytesToSend;
  bool TxReady;
  bool NewCmdsFlag;
/// DEBUG ONLY
  void PrintHex8(uint_least8_t *data, uint_least8_t length);
};

#endif
