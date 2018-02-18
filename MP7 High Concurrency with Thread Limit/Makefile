# makefile

all: dataserver client

reqchannel.o: reqchannel.h reqchannel.cpp
	g++ -std=c++11 -c -g reqchannel.cpp

BoundedBuffer.o: BoundedBuffer.h BoundedBuffer.cpp
	g++ -std=c++11 -g -c BoundedBuffer.cpp

dataserver: dataserver.cpp reqchannel.o 
	g++ -std=c++11 -g -o dataserver dataserver.cpp reqchannel.o -lpthread

client: client_MP7.cpp reqchannel.o BoundedBuffer.o
	g++ -std=c++11 -g -o client client_MP7.cpp reqchannel.o BoundedBuffer.o -lpthread -lrt

clean:
	rm -rf *.o fifo* *.DS_Store dataserver client output*
