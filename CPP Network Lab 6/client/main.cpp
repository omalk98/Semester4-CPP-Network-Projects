#include <iostream>
#include <string>
#include "../socket.h"

int main() {
	std::string message = "";

	Client_UDP client;
	client.create_socket();
	client.send_message("Here it goes!", "127.0.0.1", 27000);
	struct sockaddr_in SvrAddr;
	SvrAddr = client.receive_message(message);
	std::cout << "Message Received: " << message << std::endl;

	return 0;
}
