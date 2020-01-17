#include "client.hpp"
#include "cmd.hpp"
#include "../message/message.hpp"
#include "socketClient.hpp"
#include <iostream>

using namespace std;
extern Client * g_client;

void SendCmd::cmdExec(vector<string>& argv) {
    cout << "\nsendCmd";
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
        cout << "\nyou are already signIn";
        return;
    }
    string userName;
    string password;
    string noop;
    cout << "\nPlease Enter the userName: ";
    cin >> userName;
    cout << "Please Enter the password: ";
    cin >> password;
    getline(cin, noop);
    Message * msg = new SignUpMsg(userName, password);
    char charAck[MAX_MSG_SIZE];
    sendAndWaitAck(g_client->_socketFd, msg, charAck);
    SignUpMsg * ack = (SignUpMsg *)charAck;
    switch(ack->_signUpAck){
        case SIGNUP_ACK_SUCCESS:
            g_client->_clientState = SIGNIN_CLIENT;
            g_client->_userName = userName;
            g_client->buildDataConn();
            break;
        case SIGNUP_ACK_FAIL:
            cout << "username " << ack->_userName << " is already used\n";
            break;
    }
    delete msg;
}

void SignInCmd::cmdExec(vector<string> & argv) {
    if(g_client->_clientState != IDLE_CLIENT){
        cout << "\nyou are already signIn";
        return;
    }
    string userName;
    string password;
    string noop;
    cout << "\nPlease Enter the userName: ";
    cin >> userName;
    cout << "Please Enter the password: ";
    cin >> password;
    getline(cin, noop);
    Message * msg = new SignInMsg(userName, password);
    char charAck[MAX_MSG_SIZE];
    sendAndWaitAck(g_client->_socketFd, msg, charAck);
    SignInMsg * ack = (SignInMsg *)charAck;
    switch(ack->_signInAck){
        case SIGNIN_ACK_SUCCESS:
            g_client->_clientState = SIGNIN_CLIENT;
            g_client->_userName = userName;
            g_client->buildDataConn();
            break;
        case SIGNIN_ACK_USER_NOT_EXIST:
            cout << "username " << ack->_userName << " is not existed\n";
            break;
        case SIGNIN_ACK_PASS_NOT_MATCH:
            cout << "password is not correct\n";
            break;
    }
    delete msg;
}

void SignOutCmd::cmdExec(vector<string> & argv) {
    if(g_client->_clientState == IDLE_CLIENT){
        cout << "\nYou have not signed in";
        return;
    }

    Message * msg = new SignOutMsg(g_client->_userName);
    char charAck[MAX_MSG_SIZE];
    sendAndWaitAck(g_client->_socketFd, msg, charAck);
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
    cout << "\nretrieveCmd";
}


