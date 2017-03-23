#include "ProtobuffSerial.h"

ProtobuffSerial::ProtobuffSerial()
{
    /// - Start off in RECEIVING state.
    ActiveState = RECEIVING;
    PacketHeader = 0x534F4521;
    TxCrc32 = 0x0000;
    RxByteCounter = 0;
    NumBytesToSend = 0;
    /// - Compute the maximum time we will wait to received the full cmd packet.
    ClearBuffers();
}

ProtobuffSerial::~ProtobuffSerial() {
	/// - Nothing to do
}

int ProtobuffSerial::InitHw() {
	mySerial.begin(9600);
    Serial.begin(9600);
    while (!mySerial) {
        ; // wait for serial port to connect. Needed for native USB
    }
	return SUCCESS;
}

int ProtobuffSerial::ReadPacket() {
    while (mySerial.available() > 0) {
	    // read the incoming byte:
	    RxBuffer[RxByteCounter++] = mySerial.read();
    }
	if (RxByteCounter){
        Serial.println("Full Cmd Packet Received: ");
        PrintHex8(RxBuffer, RxByteCounter);
        Serial.println("Attempting to unpack ... ");
	 	RxByteCounter = 0;
	 	return RX_PACKET_READY;
	}
	return RX_READING_PACKET;
}

int ProtobuffSerial::WritePacket() {
    mySerial.write(TxBuffer, NumBytesToSend);
    return TX_PACKET_SUCCESS;
}

bool ProtobuffSerial::ValidCrc(){
    // /// - Compute the CRC on the command packet we received.
    // uint32_t computed_crc32 = CommCrc32::crc32(&RxBuffer[4], CommandPacket_size, 0);
    // /// - Unload the CRC32 that was sent over the wire.
    // uint32_t rcvd_crc32 = 0;
    // uint16_t crcStartIndx = 4 + CommandPacket_size;
    // uint16_t lowerWord = (RxBuffer[crcStartIndx+1] << 8) | RxBuffer[crcStartIndx];
    // uint16_t upperWord = (RxBuffer[crcStartIndx+3] << 8) | RxBuffer[crcStartIndx+2];
    // rcvd_crc32 = ((uint32_t) upperWord << 16) | lowerWord;
    // /// - Check received CRC with what was computed
    // return rcvd_crc32 == computed_crc32;

    // TODO Implement CRC checking
    return true;
}


int ProtobuffSerial::RunComm() {
    int rx_status, tx_status;

    switch(ActiveState)
    {
        case RECEIVING:
            rx_status = ReadPacket();
            if(rx_status == RX_PACKET_READY){
                ActiveState = TRANSMITTING;
            }else if (rx_status == RX_READING_PACKET){
                return RX_READING_PACKET;
            }else if (rx_status == RX_PACKET_FAIL){
                ClearBuffersAndReset();
                return RX_PACKET_FAIL;
            }
        break;

        case TRANSMITTING:
            /// - First, decode the received commands.
            if (!Decode()){
                Serial.print("Decode FAIL");
                ClearBuffersAndReset();
                ActiveState = RECEIVING;
                return UNLOAD_FAIL;
            }
            /// - Next, encode the telemetry.
            if (!Encode()){
                Serial.print("Encode FAIL");
                ClearBuffersAndReset();
                ActiveState = RECEIVING;
                return LOAD_FAIL;
            }
            /// - Now, write the packet out to the channel
            tx_status = WritePacket();
            if(tx_status == TX_PACKET_SUCCESS){
                Serial.println("TX Success");
                ActiveState = RECEIVING;
                return SUCCESS;
            }else if(tx_status == TX_PACKET_FAIL){
                Serial.println("TX Fail");
                ClearBuffersAndReset();
                ActiveState = RECEIVING;
                return TX_PACKET_FAIL;
            }
            ActiveState = RECEIVING;
        break;

        default:
            /// - invalid state - go back to RECEIVING
            ActiveState = RECEIVING;
        break;
    }
    /// - Successful cycle through the comm state machine.
    return SUCCESS;
}

bool ProtobuffSerial::Encode() {
    /// - Write the packet header to the Tx buffer.
    // WriteHeader();
    /// - Create a stream to encode the Tx buffer.
    pb_ostream_t outstream = pb_ostream_from_buffer(TxBuffer, sizeof(TxBuffer));
    /// - Encode the Tx buffer.
    if(!pb_encode(&outstream, TelemetryPacket_fields, &Telemetry))
    {
        return false;
    }
    NumBytesToSend = outstream.bytes_written;
    /// - Compute and write the CRC32 to the end of the Tx buffer.
    TxCrc32 = CommCrc32::crc32(&TxBuffer[4], outstream.bytes_written, 0);
    // WriteCrc32();
    /// - Tx Buffer is now ready to be written over the SCI channel.
    return true;
}

bool ProtobuffSerial::Decode() {
    /// - SCI hardware must fill the RxBuffer with header, data, and crc32
    ///   before this function is called.
    // if (!ValidHeader())
    // {
    //     return false;
    // }
    /// - Create a stream that reads from the receive buffer... decode from
    ///   byte 5 of the Rx Buffer to command packet size
    pb_istream_t stream = pb_istream_from_buffer(RxBuffer, RxByteCounter);
    /// - Decode the command packet from the RxBuffer
    if (!pb_decode(&stream, CommandPacket_fields, &Commands))
    {
        return false;
    }
    // /// - Check the CRC
    // if (!ValidCrc())
    // {
    //     return false;
    // }
    return true;
}

void ProtobuffSerial::WriteHeader() {
    TxBuffer[0] = (PacketHeader & 0x000000ff);
    TxBuffer[1] = (PacketHeader & 0x0000ff00) >> 8;
    TxBuffer[2] = (PacketHeader & 0x00ff0000) >> 16;
    TxBuffer[3] = (PacketHeader & 0xff000000) >> 24;
}

void ProtobuffSerial::WriteCrc32() {
    uint_least8_t crcStartIndx = 4 + TelemetryPacket_size;
    TxBuffer[crcStartIndx] = (TxCrc32 & 0x000000ff);
    TxBuffer[crcStartIndx+1] = (TxCrc32 & 0x0000ff00) >> 8;
    TxBuffer[crcStartIndx+2] = (TxCrc32 & 0x00ff0000) >> 16;
    TxBuffer[crcStartIndx+3] = (TxCrc32 & 0xff000000) >> 24;
}

bool ProtobuffSerial::ValidHeader(){
    return RxBuffer[0] == 0x21 &&
           RxBuffer[1] == 0x45 &&
           RxBuffer[2] == 0x4F &&
           RxBuffer[3] == 0x53;
}

void ProtobuffSerial::ClearBuffers(){
    for (int indx = 0; indx < COMM_MAX_BUFF_SIZE; indx++)
    {
        TxBuffer[indx] = 0x00;
        RxBuffer[indx] = 0x00;
    }
}

void ProtobuffSerial::ClearBuffersAndReset(){
    ClearBuffers();
    RxByteCounter = 0;
}


void ProtobuffSerial::PrintHex8(uint_least8_t *data, uint_least8_t length)
{
    // prints 8-bit data in hex with leading zeroes
    Serial.print("0x"); 
    for (int i=0; i<length; i++) { 
        if (data[i]<0x10) {Serial.print("0");} 
        Serial.print(data[i],HEX); 
        Serial.print(" "); 
    }
}

