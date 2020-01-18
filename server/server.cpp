#include "server.hpp"
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
#include<vector>
#include "../socket/socketCommon.hpp"
#include "../message/message.hpp"
using namespace std;

void 
Server::ackMsg(size_t socketFd, Message * msg, size_t size){
    completeSend(socketFd, msg, size);
}

void
Server::removeConnection(size_t socketFd){
    ConnectionMap::iterator it = _connectionMap.begin();
    while(it != _connectionMap.end()){
        if(it->second == socketFd){
            _connectionMap.erase(it);
            it = _dataConnMap.find(it->first);
            assert(it != _dataConnMap.end());
            _dataConnMap.erase(it);
            return;
        }
        ++it;
    }
    it = _dataConnMap.begin();
    while(it != _dataConnMap.end()){
        if(it->second == socketFd){
            _dataConnMap.erase(it);
            it = _connectionMap.find(it->first);
            assert(it != _connectionMap.end());
            _connectionMap.erase(it);
            return;
        }
        ++it;
    }
}

void 
Server::serverListen(){
	signal(SIGPIPE, SIG_IGN);
	if((_masterSocketFd = socket(AF_INET, SOCK_STREAM, 0)) == 0){
		perror("socket error");
		exit(1);
	}

	struct sockaddr_in svr_addr;
	svr_addr.sin_family = AF_INET;
	svr_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	svr_addr.sin_port = htons(_listenPort);

	if(bind(_masterSocketFd, (struct sockaddr *)&svr_addr, sizeof(svr_addr)) < 0){
		perror("bind fail");
		exit(1);
	}
	printf("socket_fd: %d, Listen on port %d\n", _masterSocketFd, _listenPort);
	if(listen(_masterSocketFd, 5) < 0){
		perror("listen fail");
		exit(1);
	}

	puts("Waiting for connection...");
	fd_set readfds;
	FD_ZERO(&readfds);
	FD_SET(_masterSocketFd, &readfds);
	int max_fd = _masterSocketFd;
	int ret;
	struct sockaddr_in client;
	int client_len = sizeof(client);
	char buf[MAX_MSG_SIZE + 40];
	int read_len;
	while(1){
        memset(buf, 0, sizeof(buf));
		fd_set working_readfds;
		int working_max_fd = max_fd;
		memcpy(&working_readfds, &readfds, sizeof(fd_set));
		if((ret = select(max_fd + 1, &working_readfds, NULL, NULL, NULL)) < 0){
			perror("select error");
			exit(1);
		}

        // new connection
		if(FD_ISSET(_masterSocketFd, &working_readfds)){
			int new_socket_fd;
			if((new_socket_fd = accept(_masterSocketFd, (struct sockaddr *)&client, (socklen_t *)&client_len)) < 0){
				perror("accept fail");
				exit(1);
			}
			printf("New connection , socket fd is %d , ip is : %s , port : %d\n" ,
					new_socket_fd , inet_ntoa(client.sin_addr) , ntohs(client.sin_port));
			FD_SET(new_socket_fd, &readfds);
			if(new_socket_fd > max_fd) max_fd = new_socket_fd;
		}

        // msg from clients
		for(int fd = 0 ; fd <= working_max_fd ; ++fd){
			if(fd == _masterSocketFd) continue;
			if(FD_ISSET(fd, &working_readfds)){
				getpeername(fd , (struct sockaddr*)&client, (socklen_t*)&client_len);   
				ChunkVec chunkVec;
                if((read_len = completeRead(fd, buf, sizeof(buf), chunkVec)) == 0){
                    printf("Host disconnected , ip %s , port %d \n" ,  
							inet_ntoa(client.sin_addr) , ntohs(client.sin_port));   
                    //Close the socket and mark as 0 in list for reuse  
                    removeConnection(fd);
                    close(fd);
					FD_CLR(fd, &readfds);
				}
				else{
					printf("recv from %s:%d\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));
                    dispatch(fd, chunkVec);
                    forwardMessage();
				}
			}
		}
	}
}

void 
Server::receiveCompleteMsg(){

}

void 
Server::dispatch(size_t socketFd, ChunkVec & chunkVec){
    
    // TODO big file case
    Chunk * chunk = chunkVec[0];
    size_t chunkSize = chunk->_size;
    size_t dataSize = chunkSize;
    char * data = new char[chunkSize];
    memcpy(data, chunk->_data, chunkSize);
    delete chunk;
    Message * message = (Message *)data;
    cout << message->_header._msgType << endl;

    ServiceMap::iterator it = _serviceMap.find(message->_header._msgType);
    if(it == _serviceMap.end()){
        cout << "Unhandled service type" << endl;
        exit(1);
    }
    Service * service = it->second;
    service->service(socketFd, message);
    delete[] data;
}

void
Server::addToMsgDataMap(ChatMsg* msg, string userName){
    MsgDataMap::iterator it = _msgDataMap.find(userName);
    if(it == _msgDataMap.end()){
        vector<Message *> * vec = new vector<Message *>;
        vec->push_back(msg);
        _msgDataMap.insert(MsgDataMapPair(userName, vec));
    }
    else{
        vector<Message *> * vec = it->second;
        vec->push_back(msg);
    }

}

void
Server::forwardMessage(){
    InboxMap::iterator it = _inboxMap.begin();
    while(it != _inboxMap.end()){
        string userName = it->first;
        ConnectionMap::iterator it_conn = _dataConnMap.find(userName);
        if(it_conn != _dataConnMap.end()){
            size_t dataConnSockFd = it_conn->second;
            Inbox& inbox = it->second;
            while(!inbox.empty()){
                Message * msg = inbox.front();
                if(msg->getMsgType() == SEND_MSG){
                    sendMessage(dataConnSockFd, msg, sizeof(ChatMsg));
                }
                else if(msg->getMsgType() == SEND_FILE){
                    sendMessage(dataConnSockFd, msg, sizeof(FileMsg));
                }
                else{
                    cout << "\nUnhandle msg type" << endl;
                    exit(1);
                }
                addToMsgDataMap((ChatMsg *)msg, string(reinterpret_cast<char *>(msg->_header._sender)));
                addToMsgDataMap((ChatMsg *)msg, string(reinterpret_cast<char *>(msg->_header._receiver)));
                inbox.pop();
            }
        }
        ++it;
    }
}
