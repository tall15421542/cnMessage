#include "service.hpp"
#include "server.hpp"

extern Server * g_server;
void 
FileService::service(size_t socketFd, Message * msg){

}

void 
FileService::ackClient(){
}

void 
MsgService::service(size_t socketFd, Message * msg){

}

// util

UserInfoStatus userInfoStatus(string userName, string password){
    UserInfoMap::iterator it = g_server->_userInfoMap.find(userName);
    if(it == g_server->_userInfoMap.end()){
        cout << userName << " not exist" << endl;
        return USER_NOT_EXIST;
    }
    if(it->second != password){
        cout << userName << " password mismatch" << endl;
        return USER_PASS_NOT_MATCH;
    }
    cout << userName << "and passord is correct" << endl;
    return USER_MATCH;
}

void 
MsgService::ackClient(){

}

// class SignInService 
void 
SignInService::service(size_t socketFd, Message * msg){

}
void 
SignInService::ackClient(){
}

// class SignUpService 
void 
SignUpService::service(size_t socketFd, Message * msg){
    SignUpMsg * signUpMsg = (SignUpMsg * )msg;
    string userName(reinterpret_cast<char *>(signUpMsg->_userName));
    string password(reinterpret_cast<char *>(signUpMsg->_password));
    UserInfoStatus status = userInfoStatus(userName, password);
    switch(status){
        case USER_NOT_EXIST:
            g_server->_userInfoMap.insert(UserInfoMapPair(userName, password));
            ackClient(socketFd, SIGNUP_ACK_SUCCESS, signUpMsg);
            break;
        default:
            ackClient(socketFd, SIGNUP_ACK_FAIL, signUpMsg);
    }
} 
void SignUpService::ackClient(size_t sockFd, SignUpAck ackStatus, SignUpMsg * ack){
    ack->_header._isAck = true;
    ack->_signUpAck = ackStatus;    
    g_server->ackMsg(sockFd, ack, sizeof(SignUpMsg)); 
}

// class SignOutService
void 
SignOutService::service(size_t socketFd, Message * msg){

}
void 
SignOutService::ackClient(){

}

// class HistoryService
void 
HistoryService::service(size_t socketFd, Message * msg){

}
void 
HistoryService::ackClient(){

}


