#include "client.hpp"
#include "cmd.hpp"
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

void RegisterCmd::cmdExec(vector<string> & argv){
    cout << "\nregisterCmd";
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

