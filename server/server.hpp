#include "../message/message.hpp"
#include "../socket/socketCommon.hpp"
#include<queue>
#include<vector>
#include<map>
#include<string>
#include "service.hpp"

using namespace std;

typedef map<string, size_t> ConnectionMap;
typedef pair<string, size_t> ConnectionMapPair; 
typedef map<MsgType, Service *> ServiceMap;
typedef pair<MsgType, Service *> ServiceMapPair;
typedef map<string, string> UserInfoMap; 
typedef pair<string, string> UserInfoMapPair;

class Server{
public:
    Server():_listenPort(LISTEN_PORT){
        _serviceMap.insert(ServiceMapPair(SEND_MSG, new MsgService));
        _serviceMap.insert(ServiceMapPair(SEND_FILE, new FileService));
        _serviceMap.insert(ServiceMapPair(SIGNIN, new SignInService));
        _serviceMap.insert(ServiceMapPair(SIGNUP, new SignUpService));
        _serviceMap.insert(ServiceMapPair(SIGNOUT, new SignOutService));
        _serviceMap.insert(ServiceMapPair(BUILD_DATA_CONN, new BuildDataConnService));
    }
    void serverListen();
    void receiveCompleteMsg();
    void dispatch(size_t socketFd, ChunkVec & chunkVec);
    void ackMsg(size_t socketFd, Message * msg, size_t size);
    void removeConnection(size_t socketFd);

    UserInfoMap _userInfoMap;
    queue<Message> _messageQ;
    vector<Message> _meesageVector;
    ConnectionMap _connectionMap;
    ConnectionMap _dataConnMap;
    ServiceMap _serviceMap;
    size_t _listenPort;
    size_t _masterSocketFd;
};


