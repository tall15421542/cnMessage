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
    if(g_client->_clientState != IDLE){
        cout << "\n you are already signIn";
        return;
    }
    string userName;
    string password;
    cout << " Please Enter the userName: ";
    cin >> userName;
    cout << " Please Enter the password: ";
    cin >> password;
    Message * msg = new SignUpMsg(userName, password);
    char reply[MAX_MSG_SIZE];
    sendAndWaitAck(g_client->_socketFd, msg, reply);
}

void LoginCmd::cmdExec(vector<string> & argv) {
    cout << "\nloginCmd";
}

void LogoutCmd::cmdExec(vector<string> & argv) {
    cout << "\nlogoutCmd";
}

void RetrieveCmd::cmdExec(vector<string> & argv) {
    cout << "\nretrieveCmd";
}


