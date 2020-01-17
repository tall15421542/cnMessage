#include<iostream>
#include<thread>
#include "client.hpp"
using namespace std;

Client * g_client = new Client;

int main(){
    g_client->run();
    exit(0); 
}
