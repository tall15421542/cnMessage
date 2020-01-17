#include "../message/message.hpp"
#include<queue>
#include<vector>
#include<map>
#include<string>
#include "service.hpp"

#define LISTEN_PORT 3000

using namespace std;

typedef map<string, size_t> ConnectionMap;
typedef pair<string, size_t> ConnectionMapPair; 
typedef map<MsgType, Service *> ServiceMap;
typedef map<MsgType, Service *> ServiceMapPair;
typedef Chunk * ChunkElement;
typedef vector<ChunkElement> ChunkVec;

class Server{
public:
    Server():_listenPort(LISTEN_PORT){}
    void serverListen();
    void receiveCompleteMsg();
    void dispatch(size_t socketFd, ChunkVec & chunkVec);
    
    queue<Message> _messageQ;
    vector<Message> _meesageVector;
    ConnectionMap _connectionMap;
    ServiceMap _serviceMap;
    size_t _listenPort;
    size_t _masterSocketFd;
};


