# CFLAGS = -O3 -std=c++11
CFLAGS = -g -Wall -std=c++11


clientRun: main.o client.o cmdCharDef.o cmd.o
	g++ -o $@ $(CFLAGS) main.o client.o cmdCharDef.o cmd.o

main.o: main.cpp cmdCharDef.o client.o cmd.o
	g++ -c $(CFLAGS) main.cpp

client.o: cmdCharDef.o cmd.o client.hpp client.cpp 
	g++ -c $(CFLAGS) client.cpp

cmd.o: cmd.cpp cmd.hpp 
	g++ -c $(CFLAGS) cmd.cpp

cmdCharDef.o: cmdCharDef.cpp cmdCharDef.h
	g++ -c $(CFLAGS) cmdCharDef.cpp

clean: 
	rm *.o clientRun
