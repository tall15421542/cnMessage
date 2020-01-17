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
    SignInMsg * signInMsg = (SignInMsg * )msg;
    string userName(reinterpret_cast<char *>(signInMsg->_userName));
    string password(reinterpret_cast<char *>(signInMsg->_password));
    UserInfoStatus status = userInfoStatus(userName, password);
    switch(status){
        case USER_NOT_EXIST:
            ackClient(socketFd, SIGNIN_ACK_USER_NOT_EXIST, signInMsg);
            break;
        case USER_PASS_NOT_MATCH:
            ackClient(socketFd, SIGNIN_ACK_PASS_NOT_MATCH, signInMsg);
            break;
        case USER_MATCH:
            ackClient(socketFd, SIGNIN_ACK_SUCCESS, signInMsg);
            g_server->_connectionMap.insert(ConnectionMapPair(userName, socketFd));
            break;
        default:
            cout << "\nUnhandle userInfo status" << endl;
            exit(1);
    }
}
void 
SignInService::ackClient(size_t sockFd, SignInAck ackStatus, SignInMsg * ack){
    ack->_header._isAck = true;
    ack->_signInAck = ackStatus;    
    g_server->ackMsg(sockFd, ack, sizeof(SignInMsg)); 
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
            g_server->_connectionMap.insert(ConnectionMapPair(userName, socketFd));
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
SignOutService::service(size_t sockFd, Message * msg){
    SignOutMsg * signOutMsg = (SignOutMsg * )msg;
    string userName(reinterpret_cast<char *>(signOutMsg->_userName));
    ConnectionMap::iterator it = g_server->_connectionMap.find(userName);
    if(it != g_server->_connectionMap.end()){
        ackClient(sockFd, SIGNOUT_ACK_SUCCESS, signOutMsg);
    }
}
void 
SignOutService::ackClient(size_t sockFd, SignOutAck ackStatus, SignOutMsg * ack){
    ack->_header._isAck = true;
    ack->_signOutAck = ackStatus;    
    g_server->ackMsg(sockFd, ack, sizeof(SignOutMsg)); 
}

// class HistoryService
void 
HistoryService::service(size_t socketFd, Message * msg){

}
void 
HistoryService::ackClient(){

}

// class BuildDataConn
void 
BuildDataConnService::service(size_t sockFd, Message * msg){
    BuildDataConnMsg * connMsg = (BuildDataConnMsg *)msg;
    string userName(reinterpret_cast<char *>(connMsg->_userName));
    ConnectionMap::iterator it = g_server->_dataConnMap.find(userName);
    if(it == g_server->_dataConnMap.end()){
      cout << "Build data conn success for " << userName << endl;
       g_server->_dataConnMap.insert(ConnectionMapPair(userName, sockFd));
       ackClient(sockFd, DATA_CONN_SUCCESS, connMsg); 
    }else{
        ackClient(sockFd, DATA_CONN_DUP, connMsg);
    }
}
void 
BuildDataConnService::ackClient(size_t sockFd, BuildDataConnAck ackStatus, BuildDataConnMsg * ack){
    ack->_header._isAck = true;
    ack->_buildDataConnAck = ackStatus;    
    g_server->ackMsg(sockFd, ack, sizeof(BuildDataConnMsg)); 
}
