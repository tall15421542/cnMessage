#include "../socket/socketCommon.hpp"
#include "../message/message.hpp"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<sys/socket.h>
#include<netinet/in.h> // sockaddr_in
#include<arpa/inet.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/select.h>
#include<unistd.h>
#include<signal.h>
#include<assert.h>
#include<iostream>
size_t completeRead(size_t socketFd, char * buffer, size_t size, ChunkVec & chunkVec){
    size_t readLen = read(socketFd, buffer, size);
    if(readLen > 0){
        char * chunk = new char[readLen];
        memcpy(chunk, buffer, readLen);
        chunkVec.push_back(new Chunk(chunk, readLen));
    }
    // big msg
    /*
    Message * msg = (Message *)buffer;
    while(!msg->completePacketDelivered()){
        size_t len = read(socketFd, buffer, size);
        msg = (Message *)buffer;
        char * chunk = new char(len + 1);
        memcpy(chunk, buffer, len);
        chunk[len] = 0;
        chunkVec.push_back(new Chunk(chunk, len));
        readLen += len;
    }*/
    return readLen;
}

size_t completeSend(size_t socketFd, void * buffer, size_t size){
    return send(socketFd, buffer, size, 0);
}
