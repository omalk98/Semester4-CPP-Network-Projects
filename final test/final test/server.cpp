#include <iostream>
#include <string>
#include <thread>
#include "socket.h"

void Run(Server_TCP *server, int socket) {
	server->client_thread(socket);
}

int main() {
	Server_TCP server("127.0.0.1", 27000);

	server.create_socket();
	server.bind_socket();
	server.listen_for_connections();

	int socket = 0;

	while (true) {
		std::cout << "Waiting for a client to connect..." << std::endl;
		socket = server.accept_connection();
		std::thread(Run, &server, socket).detach();
	}
	
	return 0;
}