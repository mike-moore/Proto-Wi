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
#include "CommCrc32.h"
#include "pb_encode.h"
#include "pb_decode.h"
#include "comm_packet.pb.h"

#define COMM_MAX_BUFF_SIZE 100

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
    TX_PACKET_SUCCESS = 4,
    RX_PACKET_READY = 3,
    RX_READING_PACKET = 2,
    RX_WAITING_TO_READ = 1,
    SUCCESS = 0,
    HW_INIT_FAIL = -1,
    RX_PACKET_FAIL = -2,
    UNLOAD_FAIL = -3,
    LOAD_FAIL = -4,
    TX_PACKET_FAIL = -5,
    RESETTING = -6,
  }CommStatusCode;

  typedef enum CommStates
  {
    RECEIVING = 0,
    TRANSMITTING = 1
  }CommunicationStates;

  virtual int InitHw();
  virtual int RunComm();
  inline int CommState() { return ActiveState; }

  CommandPacket Commands;
  TelemetryPacket Telemetry;

 protected:
  SoftwareSerial mySerial = SoftwareSerial(2,3);
  virtual int ReadPacket();
  virtual int WritePacket();
  virtual bool ValidCrc();
  virtual bool Encode();
  virtual bool Decode();
  virtual void WriteHeader();
  virtual void WriteCrc32();
  virtual bool ValidHeader();
  virtual void ClearBuffers();
  virtual void ClearBuffersAndReset();
  CommunicationStates ActiveState;
  uint32_t PacketHeader;
  uint_least8_t TxBuffer[COMM_MAX_BUFF_SIZE];
  uint_least8_t RxBuffer[COMM_MAX_BUFF_SIZE];
  uint32_t TxCrc32;
  uint_least8_t RxByteCounter;
  uint_least8_t NumBytesToSend;
/// DEBUG ONLY
  void PrintHex8(uint_least8_t *data, uint_least8_t length);
};

#endif
