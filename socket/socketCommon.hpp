#ifndef SOCKET_COMMON_HEADER
#define SOCKET_COMMON_HEADER
#include<vector>
#include<../message/message.hpp>
#define SERVER_ADDR "localhost"
#define SERVER_PORT 3002
#define LISTEN_PORT SERVER_PORT
typedef Chunk * ChunkElement;
typedef vector<ChunkElement> ChunkVec;
size_t completeRead(size_t socketFd, char * buffer, size_t size, ChunkVec & chunkVec);
size_t completeSend(size_t socketFd, void * buffer, size_t size);
int sendMessage(size_t sockFd, Message * message,size_t size);
#endif
