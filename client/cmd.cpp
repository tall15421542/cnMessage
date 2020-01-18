#include "client.hpp"
#include "cmd.hpp"
#include "../message/message.hpp"
#include "../socket/socketClient.hpp"
#include <iostream>
#include <assert.h>
#include <set>

using namespace std;
extern Client * g_client;

void UpdateCmd::cmdExec(vector<string>& argv){
   if(g_client->_clientState == IDLE_CLIENT){
        cout << "\nplease sign in first" << endl;
        return;
   }
   UpdateMsg * msg = new UpdateMsg(g_client->_userName);
   vector<char *>charAckVec;
   sendAndWaitAckStatus(g_client->_socketFd, msg, sizeof(UpdateMsg), UPDATE_COMPLETE, charAckVec);
   for(size_t i = 0 ; i < charAckVec.size() ; ++i){
     g_client->_messageRecord.push_back((Message *)charAckVec[i]);
   }
   cout << "\nYou have " << charAckVec.size() - 1 << "unread message";
}


void ReadCmd::cmdExec(vector<string>& argv){
    for(size_t i = 0 ; i < g_client->_messageRecord.size() ; ++i){
        ChatMsg * msg = (ChatMsg *)g_client->_messageRecord[i];
        string sender(msg->_header._sender);
        string receiver(msg->_header._receiver);
        string key;
        if(sender != g_client->_userName){
            key = sender;
        }
        else{
            key = receiver;
        }
        MsgRecordMap::iterator it = g_client->_msgRecordMap.find(key);
        if(it != g_client->_msgRecordMap.end()){
            it->second->push_back(msg);
        }
        else{
            vector<Message *> * vec = new vector<Message *>;
            vec->push_back(msg);
            g_client->_msgRecordMap.insert(MsgRecordMapPair(key, vec));
        }
    }
    g_client->_messageRecord.clear();
    if(argv[1] == "-l"){
        MsgRecordMap::iterator it = g_client->_msgRecordMap.begin();
        cout << "\n";
        while(it != g_client->_msgRecordMap.end()){
            cout << it->first << endl;
            ++it;
        }
        return;
    }
    cout << "\n";
    for(int i = 1 ; i < argv.size() ; ++i){
        MsgRecordMap::iterator it = g_client->_msgRecordMap.find(argv[i]);
        vector<Message *> * vec = it->second;
        cout << argv[i] << " #######################\n";
        for(size_t i = 0 ; i < vec->size() ; ++i){
            ChatMsg * msg = (ChatMsg*)(*vec)[i];
            cout << msg->_header._sender << ": ";
            cout << string(reinterpret_cast<char *>(msg->_buf)) << endl;
        }
        
    }
}

void SendCmd::cmdExec(vector<string>& argv) {
    assert(argv.size() <= 2);
    if(g_client->_clientState == IDLE_CLIENT){
        cout << "\nplease sign in first" << flush;
        return;
    }

    if(argv.size() == 2){
        g_client->_personTalking = argv[1];
        g_client->_clientState = TALKING_CLIENT;
    }else{
        if(g_client->_clientState != TALKING_CLIENT){
            cout << "\nunknown reciever" << flush;
            return;
        }
    }

    string msgContent, noop;
    cout << "\nreceiver " << g_client->_personTalking << ": " << flush;
    getline(cin ,msgContent);
    ChatMsg * chatMsg = new ChatMsg(g_client->_userName, g_client->_personTalking, msgContent);
    char charAck[MAX_MSG_SIZE];
    sendAndWaitAck(g_client->_socketFd, chatMsg, sizeof(ChatMsg), charAck);
    ChatMsg * ack = (ChatMsg *)charAck;
    switch(ack->_chatAck){
        case CHAT_REICEIER_NOT_EXIST:
            cout <<"\nreceiver is not exist" << flush;
            break;
        case CHAT_SENT:
            break;
        default:
            cout << "\nUnhandle chat ack\n" << flush;
            exit(1);
    }
}



void SendMessageCmd::sendHeader() {

    printf("\nsendCmd");
}

void SendMessageCmd::sendContent() {

}

void SendFileCmd::sendHeader() {

}

void SendFileCmd::sendContent() {

}

void SignUpCmd::cmdExec(vector<string> & argv){
    if(g_client->_clientState != IDLE_CLIENT){
        cout << "\nyou are already signIn" << flush;
        return;
    }
    string userName;
    string password;
    string noop;
    cout << "\nPlease Enter the userName: " << flush;
    cin >> userName;
    cout << "Please Enter the password: " << flush;
    cin >> password;
    getline(cin, noop);
    Message * msg = new SignUpMsg(userName, password);
    char charAck[MAX_MSG_SIZE];
    sendAndWaitAck(g_client->_socketFd, msg, sizeof(SignUpMsg), charAck);
    SignUpMsg * ack = (SignUpMsg *)charAck;
    switch(ack->_signUpAck){
        case SIGNUP_ACK_SUCCESS:
            g_client->_clientState = SIGNIN_CLIENT;
            g_client->_userName = userName;
            g_client->buildDataConn();
            break;
        case SIGNUP_ACK_FAIL:
            cout << "username " << ack->_userName << " is already used\n" << flush;
            break;
    }
    delete msg;
}

void SignInCmd::cmdExec(vector<string> & argv) {
    if(g_client->_clientState != IDLE_CLIENT){
        cout << "\nyou are already signIn" << flush;
        return;
    }
    string userName;
    string password;
    string noop;
    cout << "\nPlease Enter the userName: " << flush;
    cin >> userName;
    cout << "Please Enter the password: " << flush;
    cin >> password;
    getline(cin, noop);
    Message * msg = new SignInMsg(userName, password);
    char charAck[MAX_MSG_SIZE];
    sendAndWaitAck(g_client->_socketFd, msg, sizeof(SignInMsg), charAck);
    SignInMsg * ack = (SignInMsg *)charAck;
    switch(ack->_signInAck){
        case SIGNIN_ACK_SUCCESS:
            g_client->_clientState = SIGNIN_CLIENT;
            g_client->_userName = userName;
            g_client->buildDataConn();
            break;
        case SIGNIN_ACK_USER_NOT_EXIST:
            cout << "username " << ack->_userName << " is not existed\n" << flush;
            break;
        case SIGNIN_ACK_PASS_NOT_MATCH:
            cout << "password is not correct\n" << flush;
            break;
    }
    delete msg;
}

void SignOutCmd::cmdExec(vector<string> & argv) {
    if(g_client->_clientState == IDLE_CLIENT){
        cout << "\nYou have not signed in" << flush;
        return;
    }

    Message * msg = new SignOutMsg(g_client->_userName);
    char charAck[MAX_MSG_SIZE];
    sendAndWaitAck(g_client->_socketFd, msg, sizeof(SignOutMsg), charAck);
    SignOutMsg * ack = (SignOutMsg *)charAck;
    if(ack->_signOutAck == SIGNOUT_ACK_SUCCESS){
        g_client->_userName = "unknown";
        g_client->_clientState = IDLE_CLIENT;
        g_client->closeDataConn();
    }
    else{
        cout << "Unhandle signout status" << endl;
    }

    delete msg;
}

void RetrieveCmd::cmdExec(vector<string> & argv) {
    cout << "\nretrieveCmd" << flush;
}


