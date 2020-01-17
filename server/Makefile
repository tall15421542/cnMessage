# CFLAGS = -O3 -std=c++11
CFLAGS = -g -Wall -std=c++11

serverRun: main.o server.o service.o 
	g++ -o $@ $(CFLAGS) main.o server.o service.o 

main.o: main.cpp server.o service.o 
	g++ -c $(CFLAGS) main.cpp

server.o: service.o server.cpp server.hpp
	g++ -c $(CFLAGS) server.cpp

service.o: service.hpp service.cpp ../message/message.hpp
	g++ -c $(CFLAGS) service.cpp

clean: 
	rm *.o serverRun
