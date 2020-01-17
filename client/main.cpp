#include<iostream>
#include "client.hpp"
using namespace std;

Client * g_client = new Client;

int main(){
  while(1){
    g_client->readCmd();
  }
}
