#include <iostream>
#include <string>
#include "../socket.h"

int main() {

	data_packet my_packet = create_packet();

	//these lines are useful to test your serializer and deserializer
	//without requiring to run two applications
	struct serialized_packet serialized_data = data_packet_serializer(my_packet);
	data_packet recovered = data_packet_deserializer(serialized_data.data);

	std::cout << "original: " << my_packet.data << std::endl;
	std::cout << "serialized: " << serialized_data.data << std::endl;
	std::cout << "deserialized: " << recovered.data << std::endl;

	std::cout << "CRC: " << calculate_crc(my_packet) << " | Bit value : ";
	display(calculate_crc(my_packet));
	std::cout << std::endl;
	std::string message;
	Client_TCP client;
	client.create_socket();
	client.connect_socket("127.0.0.1", 12700);
	client.send_data_packet(my_packet);
	client.receive_message(message);
	std::cout << "Message Received: " << message << std::endl;

	return 0;
}
