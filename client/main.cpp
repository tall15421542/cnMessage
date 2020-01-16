#include<iostream>
#include "client.hpp"
using namespace std;

Client * g_client = new Client;

int main(){
  Client * client = new Client();
  while(1){
    client->readCmd();
  }
}
