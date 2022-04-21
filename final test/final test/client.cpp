#include <iostream>
#include <string>
#include <thread>
#include "socket.h"

void receiving_messages(Client_TCP*);
void sending_messages(Client_TCP*);
void receiving_packets(Client_TCP*);
void sending_packets(Client_TCP*);
void receiving_data_packets(Client_TCP*);
void sending_data_packets(Client_TCP*);

int main() {
	Client_TCP client;
	client.create_socket();

	if (!client.connect_socket("127.0.0.1", 27000)) return -1;
	std::string message;
	client.receive_message(message);
	std::cout << message << std::endl;
	// messages
	std::thread(receiving_messages, &client).detach();
	std::thread t(sending_messages, &client);
	t.join();

	// packets
	/*std::thread(receiving_packets, client).detach();
	std::thread t = std::thread(sending_messages, client);
	t.join();*/

	// data packets
	/*std::thread(receiving_data_packets, client).detach();
	std::thread t = std::thread(sending_messages, client);
	t.join();*/

	return 0;
}

void receiving_messages(Client_TCP *client) {
	std::string message;
	while (true) {
		if (client->receive_message(message) == SOCKET_ERROR) return;
		std::cout << message << std::endl;
	}
}

void sending_messages(Client_TCP *client) {
	std::string message;
	while (true) {
		std::cout << "Enter a string ([q] to quit):" << std::endl;
		std::getline(std::cin, message);
		if (client->send_message(message) == SOCKET_ERROR) break;
		if (message == "[q]") break;
	}
}

void receiving_packets(Client_TCP *client) {
	packet packet;
	while (true) {
		if (client->receive_packet(packet) == SOCKET_ERROR) return;
		std::cout << packet.letters << std::endl;
	}
}

void sending_packets(Client_TCP *client) {
	packet packet;
	while (true) {
		packet = create_packet();
		if (client->send_packet(packet) == SOCKET_ERROR) break;
	}
}

void receiving_data_packets(Client_TCP *client) {
	data_packet dataPacket;
	while (true) {
		if (client->receive_data_packet(dataPacket) == SOCKET_ERROR) return;
		std::cout << dataPacket.numbers << std::endl;
	}
}

void sending_data_packets(Client_TCP *client) {
	data_packet dataPacket;
	while (true) {
		dataPacket = create_data_packet();
		if (client->send_data_packet(dataPacket) == SOCKET_ERROR) break;
	}
}