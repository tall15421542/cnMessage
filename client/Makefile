# CFLAGS = -O3 -std=c++11
CFLAGS = -g -Wall -std=c++11
LIB=-lm -lbsd
INCLUDE=-I../socket -I../message

clientRun: main.o client.o cmdCharDef.o cmd.o
	g++ -o $@ $(CFLAGS) main.o client.o cmdCharDef.o cmd.o socketClient.o $(LIB) $(INCLUDE) 

main.o: main.cpp cmdCharDef.o client.o cmd.o
	g++ -c $(CFLAGS) main.cpp $(INCLUDE)

client.o: cmdCharDef.o cmd.o client.hpp client.cpp 
	g++ -c $(CFLAGS) client.cpp $(INCLUDE)

cmd.o: cmd.cpp cmd.hpp socketClient.o 
	g++ -c $(CFLAGS) cmd.cpp $(INCLUDE)

socketClient.o: ../message/message.hpp ../socket/socketClient.cpp ../socket/socketClient.hpp
	g++ -c $(CFLAGS) ../socket/socketClient.cpp $(INCLUDE)


cmdCharDef.o: cmdCharDef.cpp cmdCharDef.h
	g++ -c $(CFLAGS) cmdCharDef.cpp

clean: 
	rm *.o clientRun
