#include "server.hpp"
Server * g_server = new Server;

int main(){
	g_server->serverListen();
}
