# CFLAGS = -O3 -std=c++11
CFLAGS = -g -Wall -std=c++11
INCLUDE=-I../message -I../socket
serverRun: main.o server.o service.o 
	g++ -o $@ $(CFLAGS) main.o server.o service.o socketCommon.o $(INCLUDE)

main.o: main.cpp server.o service.o 
	g++ -c $(CFLAGS) main.cpp $(INCLUDE)

server.o: service.o server.cpp server.hpp
	g++ -c $(CFLAGS) server.cpp $(INCLUDE)

service.o: service.hpp service.cpp socketCommon.o ../message/message.hpp 
	g++ -c $(CFLAGS) service.cpp $(INCLUDE)

socketCommon.o: ../socket/socketCommon.hpp ../socket/socketCommon.cpp
	g++ -c $(CFLAGS) ../socket/socketCommon.cpp $(INCLUDE)

clean: 
	rm *.o serverRun
