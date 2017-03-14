# This makefile calls nanopb to auto-generate the comm inteface
ARDUINO_PROJECT_HOME = ../Arduino/ProtoWi
NANO_PB_HOME = ${ARDUINO_PROJECT_HOME}/nanopb
include ${NANO_PB_HOME}/extra/nanopb.mk

PROTO_FILE = comm_packet.proto

all: comm_packet

comm_packet:
	$(PROTOC) $(PROTOC_OPTS) --nanopb_out=$(ARDUINO_PROJECT_HOME) $(PROTO_FILE)
	protoc --python_out=../Python/ $(PROTO_FILE)

clean:
	rm -rf ${ARDUINO_PROJECT_HOME}/*.pb.* ../Python/*_pb2.py
