#ifndef SOCKET_CLIENT_HEADER
#define SOCKET_CLIENT_HEADER

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<sys/socket.h>
#include<netinet/in.h> // sockaddr_in
#include<netinet/tcp.h>
#include<arpa/inet.h>
#include<sys/time.h>
#include<sys/types.h>
#include<sys/select.h>
#include<unistd.h>
#include<pthread.h>
#include<netdb.h>
#include<fcntl.h>
#include "../message/message.hpp"

struct ip_host_port{
	char * hostname;
	char ip[INET_ADDRSTRLEN];
	int port;
};

#define t 1000.0
#define n = 0
int to_sockaddr(ip_host_port * dst);
int connect_wait(int sock_fd, struct sockaddr * addr, size_t addr_len);
int buildConnection();
void sendAndWaitAck(size_t sockFd, Message * message, size_t size,  char * reply);
void monitorSockFd(size_t sockFd);
void sendAndWaitAckStatus(size_t sockFd, Message * message, size_t size, int status, vector<char *> &chackAckVec);
#endif // SOCKET_CLIENT_HEADER
