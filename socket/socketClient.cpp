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

#include<iostream>
using namespace std;
#include "../message/message.hpp"
#include "../socket/socketCommon.hpp"
#include "../client/client.hpp"
extern Client * g_client;

#define t 1000.0
#define n = 0
int to_sockaddr(ip_host_port * dst){
	struct addrinfo hint;
	memset(&hint, 0, sizeof(hint));
	hint.ai_family = AF_INET;
	hint.ai_socktype = SOCK_STREAM;
	int status;
	struct addrinfo * res;
	if((status = getaddrinfo(SERVER_ADDR, NULL, &hint, &res)) != 0){
		return -1;
	}
	struct sockaddr_in *ip = (struct sockaddr_in *)res->ai_addr;
	void * addr = &(ip->sin_addr);
	char ipstr[INET6_ADDRSTRLEN];
	inet_ntop(res->ai_family, addr, ipstr, sizeof(ipstr));
	strcpy(dst->ip, ipstr);
	dst->hostname = (char *)malloc(sizeof(char) * strlen(SERVER_ADDR));
	strcpy(dst->hostname, SERVER_ADDR);
	dst->port = SERVER_PORT;
	freeaddrinfo(res);
	return 0;
}

int connect_wait(int sock_fd, struct sockaddr * addr, size_t addr_len){
	int res, opt;
	if((opt = fcntl(sock_fd, F_GETFL, NULL)) < 0){
		perror("fcntl get error");
		return -1;
	}

	if((res = fcntl(sock_fd, F_SETFL, opt | O_NONBLOCK)) < 0){
		perror("fcntl set error");
		return -1;
	}

	if(errno == EINPROGRESS || (res = connect(sock_fd, addr, addr_len)) < 0){
		if(errno == EINPROGRESS){
			fd_set wait_set;
			FD_ZERO(&wait_set);
			FD_SET(sock_fd, &wait_set);
			struct timeval timeout;
			timeout.tv_sec = (unsigned int)(t/1000.0);
			timeout.tv_usec = (unsigned int)((t - timeout.tv_sec * 1000) * 1000);
			res = select(sock_fd + 1, NULL, &wait_set, NULL, &timeout);
			if(res == 0){
				return -1;
			}
		}
	}else{
		res = 1;
	}
	
	// reset socket flags
	if (fcntl (sock_fd, F_SETFL, opt) < 0) {
		perror("fcntl set error");
		exit(1);
	}

	if(res < 0){
		return -1;
	}else if(res == 0){
		errno = ETIMEDOUT;
		return 1;
	}else{
		socklen_t len = sizeof(opt);
		if (getsockopt (sock_fd, SOL_SOCKET, SO_ERROR, &opt, &len) < 0) {
			return -1;
		}

		// there was an error
		if (opt) {
			errno = opt;
			return -1;
		}
	}
	return 0;
}

int buildConnection(){
	struct ip_host_port hostInfo = {};
	int cnt = 5;
	while(to_sockaddr(&hostInfo) < 0){
		--cnt;
		if(cnt == 0){
			cout << "Cannot connect to server\n" << endl;
			exit(1);
		}
		usleep(1000 * t);
	}

	int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(sock_fd == 0){
		perror("socket error");
		exit(1);
	}

	struct sockaddr_in host_addr;
	host_addr.sin_family = AF_INET;
	host_addr.sin_port = htons(hostInfo.port);
	host_addr.sin_addr.s_addr = inet_addr(hostInfo.ip);
	int res;
	cnt = 5;
	while((res = connect_wait(sock_fd, (struct sockaddr *)&host_addr, sizeof(host_addr))) < 0){
		--cnt;
		if(cnt == 0){
			usleep(t * 1000);
			printf("timeout when connect to %s:%d\n", hostInfo.ip, hostInfo.port);
			perror("connect error");
			exit(1);
		}
	}
	printf("socket(%d) connect to %s(%s):%d\n", sock_fd, hostInfo.hostname, hostInfo.ip, hostInfo.port);
	return sock_fd;
}


void sendAndWaitAck(size_t sockFd, Message * message, size_t size, char * charAck){
	fd_set wait_set;
	FD_ZERO(&wait_set);
	FD_SET(sockFd, &wait_set);
	sendMessage(sockFd, message, size);
	int res = select(sockFd + 1, &wait_set, NULL, NULL, NULL);
	size_t data_len = recv(sockFd, charAck, MAX_MSG_SIZE + 40, 0);
}

void sendAndWaitAckStatus(size_t sockFd, Message * message, size_t size, int status, vector<char *> &charAckVec){
    fd_set wait_set;
    FD_ZERO(&wait_set);
    FD_SET(sockFd, &wait_set);
    sendMessage(sockFd, message, size);
    while(1){
        FD_ZERO(&wait_set);
        FD_SET(sockFd, &wait_set);
        int res = select(sockFd + 1, &wait_set, NULL, NULL, NULL);
        char * ack = new char[MAX_MSG_SIZE + 40];
        recv(sockFd, ack, MAX_MSG_SIZE + 40, 0);
        charAckVec.push_back(ack);
        UpdateMsg * mesg = (UpdateMsg *)ack;
        if(mesg->_updateAck == UPDATE_COMPLETE) break;
    }
}

void monitorSockFd(size_t sockFd){
	fd_set wait_set;
    char charMsg[MAX_MSG_SIZE + 40];
    while(1){
        if(g_client->_clientState == IDLE_CLIENT){
            sleep(5);
            continue;
        }
        memset(charMsg, 0, sizeof(charMsg));
        FD_ZERO(&wait_set);
        FD_SET(sockFd, &wait_set);
        int res = select(sockFd + 1, &wait_set, NULL, NULL, NULL);
        size_t data_len = recv(sockFd, charMsg, sizeof(charMsg), 0);
        Message * msg = (Message *)charMsg;
        g_client->reprintCmd();
    }
}
