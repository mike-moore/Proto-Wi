#include "ProtobuffSerial.h"

ProtobuffSerial::ProtobuffSerial()
{
    TxReady = false;
    NewCmdsFlag = false;
    RxByteCounter = 0;
    NumBytesToSend = 0;
    /// - Compute the maximum time we will wait to received the full cmd packet.
    ClearBuffers();
}

ProtobuffSerial::~ProtobuffSerial() {
	/// - Nothing to do
}

void ProtobuffSerial::InitHw() {
	mySerial.begin(9600);
    Serial.begin(9600);
    while (!mySerial) {
        ; // wait for serial port to connect. Needed for native USB
    }
}

int ProtobuffSerial::ReadPacket() {
    int bytes_avail = mySerial.available();
    if (bytes_avail) {
        for (int byteNumber = 0; byteNumber < bytes_avail; byteNumber++){
            RxBuffer[RxByteCounter++] = mySerial.read();
        }
    }
    if (RxByteCounter >= 19){
	 	return RX_PACKET_READY;
	}
	return RX_READING_PACKET;
}

void ProtobuffSerial::WritePacket() {
    mySerial.write(TxBuffer, NumBytesToSend);
}

int ProtobuffSerial::Rx() {
    int rx_status;
    rx_status = ReadPacket();
    if (rx_status == RX_PACKET_FAIL){
        ClearBuffersAndReset();
    }else if( rx_status == RX_PACKET_READY){
        if (!Decode()){
            Serial.print("Decode FAIL");
            ClearBuffersAndReset();
            rx_status = UNLOAD_FAIL;
        }else{
            RxByteCounter = 0;
            NewCmdsFlag = true;
            TxReady = true;
            rx_status = RX_PACKET_SUCCESS;
        }
    }
    return rx_status;
}

int ProtobuffSerial::Tx() {
    /// - Only proceed if we are ready to transmit (we have received our full
    ///   command packet first)
    if (!TxReady){ return TX_PACKET_WAITING; }
    /// - Encode the telemetry.
    if (!Encode()){
        Serial.print("Encode FAIL");
        ClearBuffersAndReset();
        return LOAD_FAIL;
    }
    /// - Now, write the packet out to the channel
    WritePacket();
    TxReady = false;
    NewCmdsFlag = false;
    return TX_PACKET_SUCCESS;
}

bool ProtobuffSerial::Encode() {
    /// - Create a stream to encode the Tx buffer.
    pb_ostream_t outstream = pb_ostream_from_buffer(TxBuffer, sizeof(TxBuffer));
    /// - Encode the Tx buffer.
    if(!pb_encode(&outstream, TelemetryPacket_fields, &Telemetry))
    {
        return false;
    }
    NumBytesToSend = outstream.bytes_written;
    /// - Tx Buffer is now ready to be written over the SCI channel.
    return true;
}

bool ProtobuffSerial::Decode() {
    /// - Create a stream that reads from the receive buffer... decode from
    ///   byte 5 of the Rx Buffer to command packet size
    pb_istream_t stream = pb_istream_from_buffer(RxBuffer, RxByteCounter);
    /// - Decode the command packet from the RxBuffer
    if (!pb_decode(&stream, CommandPacket_fields, &Commands))
    {
        return false;
    }
    return true;
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
    TxReady = false;
    NewCmdsFlag = false;
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

