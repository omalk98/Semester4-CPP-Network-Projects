#include "socket.h"
#include <string>
#include <iostream>
#include <windows.networking.sockets.h>
#include <stdlib.h>
#pragma comment(lib, "Ws2_32.lib")

bool Node::dlls_started = false;
int Node::num_nodes = 0;

packet create_packet() {
    packet my_packet;
    std::cout << "Enter a packet name: ";
    std::cin >> my_packet.name;
    std::cout << "What is the size of the packet? ";
    std::cin >> my_packet.size;
    if (my_packet.size > 10) {
        my_packet.flag = true;
    }
    else {
        my_packet.flag = false;
    }
    my_packet.letters = new char[my_packet.size];
    for (int i = 0; i < my_packet.size; i++) {
        std::cout << "Enter element " << i << ": ";
        std::cin >> my_packet.letters[i];
    }

    return my_packet;
}

serialized_packet packet_serializer(packet my_packet) {
    //beware of Windows completing groups of four bytes
    char* my_serialized_packet = new char[16 * sizeof(char) + 1 * sizeof(bool) + 
                                1 * sizeof(int) + my_packet.size * sizeof(char)];
    char* auxptr = my_serialized_packet;
    memcpy(auxptr, my_packet.name, 16 * sizeof(char));
    auxptr += 16 * sizeof(char);
    memcpy(auxptr, &my_packet.flag, 1 * sizeof(bool));
    auxptr += 1 * sizeof(bool);
    memcpy(auxptr, &my_packet.size, 1 * sizeof(int));
    auxptr += 1 * sizeof(int);
    memcpy(auxptr, my_packet.letters, my_packet.size * sizeof(char));

    serialized_packet output;
    output.data = my_serialized_packet;
    output.length = 16 * sizeof(char) + 1 * sizeof(bool) + 1 * sizeof(int) + my_packet.size * sizeof(char);
    return output;
}

packet packet_deserializer(char* serialized_packet) {
    packet deserialized_packet;
    char* auxptr = serialized_packet;
    memcpy(deserialized_packet.name, auxptr, 16 * sizeof(char));
    auxptr += 16*sizeof(char);
    memcpy(&deserialized_packet.flag, auxptr, sizeof(bool));
    auxptr += 1 * sizeof(bool); 
    memcpy(&deserialized_packet.size, auxptr, sizeof(int));
    auxptr += 1 * sizeof(int);
    deserialized_packet.letters = new char[deserialized_packet.size];
    memcpy(deserialized_packet.letters, auxptr, sizeof(char) * deserialized_packet.size);
    return deserialized_packet;
}

data_packet create_data_packet()
{
    static int id_generator = 0;
    data_packet my_data_packet;
    my_data_packet.id = id_generator++;
    std::cout << "Enter a packet name: ";
    std::cin >> my_data_packet.name;
    std::cout << "What is the size of the packet? ";
    std::cin >> my_data_packet.size;
    my_data_packet.numbers = new int[my_data_packet.size];
    for (int i = 0; i < my_data_packet.size; i++) {
        std::cout << "Enter element " << i << ": ";
        std::cin >> my_data_packet.numbers[i];
    }
    my_data_packet.tail = my_data_packet.id * my_data_packet.size;
    return my_data_packet;
}

serialized_packet data_packet_serializer(data_packet my_data_packet)
{
    char* my_serialized_packet = new char[16 * sizeof(char) + 1 * sizeof(bool) +
        1 * sizeof(int) + my_data_packet.size * sizeof(int)];
    char* auxptr = my_serialized_packet;
    memcpy(auxptr, my_data_packet.name, 16 * sizeof(char));
    auxptr += 16 * sizeof(char);
    memcpy(auxptr, &my_data_packet.id, 1 * sizeof(int));
    auxptr += 1 * sizeof(int);
    memcpy(auxptr, &my_data_packet.size, 1 * sizeof(int));
    auxptr += 1 * sizeof(int);
    memcpy(auxptr, &my_data_packet.tail, 1 * sizeof(int));
    auxptr += 1 * sizeof(int);
    memcpy(auxptr, my_data_packet.numbers, my_data_packet.size * sizeof(int));

    serialized_packet output;
    output.data = my_serialized_packet;
    output.length = 16 * sizeof(char) + 1 * sizeof(bool) + 1 * sizeof(int) + my_data_packet.size * sizeof(int);
    return output;
}

data_packet data_packet_deserializer(char* serialized_packet)
{
    data_packet deserialized_data_packet;
    char* auxptr = serialized_packet;
    memcpy(deserialized_data_packet.name, auxptr, 16 * sizeof(char));
    auxptr += 16 * sizeof(char);
    memcpy(&deserialized_data_packet.id, auxptr, sizeof(int));
    auxptr += 1 * sizeof(int);
    memcpy(&deserialized_data_packet.size, auxptr, sizeof(int));
    auxptr += 1 * sizeof(int);
    memcpy(&deserialized_data_packet.tail, auxptr, sizeof(int));
    auxptr += 1 * sizeof(int);
    deserialized_data_packet.numbers = new int[deserialized_data_packet.size];
    memcpy(deserialized_data_packet.numbers, auxptr, sizeof(int) * deserialized_data_packet.size);
    return deserialized_data_packet;
}

Node::Node() {
    start_dlls();
    num_nodes++;
    this->active_socket = INVALID_SOCKET;
    this->protocol = "tcp";
    this->ip = "127.0.0.1";
    this->port = 27000;
}

Node::Node(std::string ip, int port) {
    start_dlls();
    num_nodes++;
    this->active_socket = INVALID_SOCKET;
    this->ip = ip;
    this->port = port;
}

Node::~Node() {
    closesocket(this->active_socket);
    std::cout << "Closing socket" << std::endl;
    num_nodes--;
    if (num_nodes == 0) {
        WSACleanup();
    }
}

void Node::start_dlls() const {
    if (!dlls_started) {
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            std::cout << "Could not start DLLs" << std::endl;
            exit(EXIT_FAILURE);
        }
        else {
            dlls_started = true;
        }
    }
}

bool Node::create_socket() {
    if (this->protocol == "tcp") {
        this->active_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    }
    else {
        this->active_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    }

    if (this->active_socket == INVALID_SOCKET) {
        std::cout << "Could not create socket" << std::endl;
        return false;
    }
    else {
        return true;
    }
}

void Node::display_info() const {
    std::cout << "IP: " << this->ip << std::endl;
    std::cout << "port: " << this->port << std::endl;
    std::cout << "protocol: " << this->protocol << std::endl;
    std::cout << "dlls: " << dlls_started << std::endl;
}

Server_TCP::Server_TCP() : Node() {
    this->role = "server";
    this->aux_socket = INVALID_SOCKET;
}

Server_TCP::Server_TCP(std::string ip, int port) : Node(ip, port) {
    this->protocol = "tcp";
    this->role = "server";
    this->aux_socket = INVALID_SOCKET;
}

Server_TCP::~Server_TCP() {
    for (int i = 0; i < MAX_SOCKETS; ++i) {
        if (active_sockets[i]) closesocket(this->client_sockets[i]);
        std::cout << "Closing client socket: [" << i << "]\n";
    }
}

bool Server_TCP::bind_socket() {
    struct sockaddr_in SvrAddr;
    SvrAddr.sin_family = AF_INET;
    SvrAddr.sin_addr.s_addr = inet_addr(this->ip.c_str());
    SvrAddr.sin_port = htons(this->port);
    if (bind(this->active_socket, (struct sockaddr*)&SvrAddr,
        sizeof(SvrAddr)) == SOCKET_ERROR) {
        std::cout << "Could not bind socket to address" << std::endl;
        return false;
    }
    else {
        return true;
    }
}

bool Server_TCP::listen_for_connections() {
    if (listen(this->active_socket, 1) == SOCKET_ERROR) {
        std::cout << "Could not start to listen" << std::endl;
        return false;
    }
    else {
        return true;
    }
}

int Server_TCP::accept_connection() {
    this->aux_socket = accept(this->active_socket, NULL, NULL);
    if (this->aux_socket == INVALID_SOCKET) {
        std::cout << "Failed to accept connection" << std::endl;
        return -1;
    }
    else {
        int sock = find_available_socket();
        if (sock > MAX_SOCKETS) {
            std::cout << "Failed to accept connection, Server Full" << std::endl;
            send_message("Sorry Server Full, try again later", sock);
            return -1;
        }
        this->client_sockets[sock] = aux_socket;
        this->active_sockets[sock] = true;
        send_message("Welcome to the Server!", sock);
        return sock;
    }
}

int Server_TCP::send_message(std::string message, int socket) {
    return send(this->client_sockets[socket], message.c_str(), message.length(), 0);
}

int Server_TCP::send_packet(packet message, int socket) {
    struct serialized_packet serialized = packet_serializer(message);
    return send(this->client_sockets[socket], serialized.data, serialized.length, 0);
}

int Server_TCP::send_data_packet(data_packet message, int socket)
{
    struct serialized_packet serialized = data_packet_serializer(message);
    return send(this->client_sockets[socket], serialized.data, serialized.length, 0);
}

int Server_TCP::receive_message(std::string& message, int socket) {
    char RxBuffer[MAX_BUFFER_SIZE] = {};
    memset(RxBuffer, 0, MAX_BUFFER_SIZE);
    int num_bytes = recv(this->client_sockets[socket], RxBuffer, MAX_BUFFER_SIZE, 0);
    message = RxBuffer;
    return num_bytes;
}

int Server_TCP::receive_packet(packet& my_packet, int socket) {
    char RxBuffer[MAX_BUFFER_SIZE] = {};
    memset(RxBuffer, 0, MAX_BUFFER_SIZE);
    int num_bytes = recv(this->client_sockets[socket], RxBuffer, MAX_BUFFER_SIZE, 0);
    my_packet = packet_deserializer(RxBuffer);
    return num_bytes;
}

int Server_TCP::receive_data_packet(data_packet& my_packet, int socket)
{
    char RxBuffer[MAX_BUFFER_SIZE] = {};
    memset(RxBuffer, 0, MAX_BUFFER_SIZE);
    int num_bytes = recv(this->client_sockets[socket], RxBuffer, MAX_BUFFER_SIZE, 0);
    my_packet = data_packet_deserializer(RxBuffer);
    return num_bytes;
}

void Server_TCP::broadcast_message(std::string message, int current_socket)
{
    for (int i = 0; i < MAX_SOCKETS; ++i) {
        if (!active_sockets[i] || i == current_socket) continue;
        send_message(message, i);
    }
}

void Server_TCP::broadcast_packet(packet packet, int current_socket)
{
    for (int i = 0; i < MAX_SOCKETS; ++i) {
        if (!active_sockets[i] || i == current_socket) continue;
        send_packet(packet, i);
    }
}

void Server_TCP::broadcast_data_packet(data_packet data_packet, int current_socket)
{
    for (int i = 0; i < MAX_SOCKETS; ++i) {
        if (!active_sockets[i] || i == current_socket) continue;
        send_data_packet(data_packet, i);
    }
}

void Server_TCP::client_thread(int socket)
{
    std::string message = "";
    //packet packet;
    //data_packet data_packet;
    while (true) {
        if (receive_message(message, socket) == SOCKET_ERROR) break;
        if (message == "[q]") break;
        broadcast_message(message, socket);
    }
    close_connection(socket);
}

int Server_TCP::find_available_socket()
{
    int socket_number = MAX_SOCKETS;
    for (int i = 0; i < MAX_SOCKETS; i++) {
        if (!active_sockets[i]) {
            socket_number = i;
            break;
        }
    }
    return socket_number;
}

void Server_TCP::close_connection(int socket) {
    closesocket(this->client_sockets[socket]);
    this->active_sockets[socket] = false;
}

Client_TCP::Client_TCP() : Node() {
    this->protocol = "tcp";
    this->role = "client";
}

bool Client_TCP::connect_socket(std::string ip, int port) {
    struct sockaddr_in SvrAddr;
    SvrAddr.sin_family = AF_INET;
    SvrAddr.sin_port = htons(port);
    SvrAddr.sin_addr.s_addr = inet_addr(ip.c_str());
    if ((connect(this->active_socket, (struct sockaddr*)&SvrAddr, sizeof(SvrAddr))) == SOCKET_ERROR) {
        std::cout << "Failed to connect to server" << std::endl;
        return false;
    }
    else {
        return true;
    }
}

int Client_TCP::send_message(std::string message) {
    return send(this->active_socket, message.c_str(), message.length(), 0);
}

int Client_TCP::send_packet(packet message){
    struct serialized_packet serialized = packet_serializer(message);
    return send(this->active_socket, serialized.data, serialized.length, 0);
}

int Client_TCP::send_data_packet(data_packet message)
{
    struct serialized_packet serialized = data_packet_serializer(message);
    return send(this->active_socket, serialized.data, serialized.length, 0);
}

int Client_TCP::receive_message(std::string& message) {
    char RxBuffer[MAX_BUFFER_SIZE] = {};
    memset(RxBuffer, 0, MAX_BUFFER_SIZE);
    int num_bytes = recv(this->active_socket, RxBuffer, MAX_BUFFER_SIZE, 0);
    message = RxBuffer;
    return num_bytes;
}

int Client_TCP::receive_packet(packet& my_packet) {
    char RxBuffer[MAX_BUFFER_SIZE] = {};
    memset(RxBuffer, 0, MAX_BUFFER_SIZE);
    int num_bytes = recv(this->active_socket, RxBuffer, MAX_BUFFER_SIZE, 0);
    my_packet = packet_deserializer(RxBuffer);
    return num_bytes;
}

int Client_TCP::receive_data_packet(data_packet& my_packet)
{
    char RxBuffer[MAX_BUFFER_SIZE] = {};
    memset(RxBuffer, 0, MAX_BUFFER_SIZE);
    int num_bytes = recv(this->active_socket, RxBuffer, MAX_BUFFER_SIZE, 0);
    my_packet = data_packet_deserializer(RxBuffer);
    return num_bytes;
}

Client_UDP::Client_UDP() : Node() {
    this->protocol = "udp";
    this->role = "client";
}

int Client_UDP::send_message(std::string message, std::string ip, int port) {
    struct sockaddr_in SvrAddr;
    SvrAddr.sin_family = AF_INET;
    SvrAddr.sin_port = htons(port);
    SvrAddr.sin_addr.s_addr = inet_addr(ip.c_str());
    return sendto(this->active_socket, message.c_str(), message.length(), 0,
        (struct sockaddr*)&SvrAddr, sizeof(SvrAddr));
}

struct sockaddr_in Client_UDP::receive_message(std::string& message) {
    struct sockaddr_in SvrAddr;
    int addr_len = sizeof(SvrAddr);

    char RxBuffer[128] = {};
    recvfrom(this->active_socket, RxBuffer, sizeof(RxBuffer), 0,
        (struct sockaddr*)&SvrAddr, &addr_len);
    message = RxBuffer;
    return SvrAddr;
}

Server_UDP::Server_UDP() : Node() {
    this->protocol = "udp";
    this->role = "server";
}

Server_UDP::Server_UDP(std::string ip, int port) : Node(ip, port) {
    this->protocol = "udp";
    this->role = "server";
}

bool Server_UDP::bind_socket() {
    struct sockaddr_in SvrAddr;
    SvrAddr.sin_family = AF_INET;
    SvrAddr.sin_addr.s_addr = inet_addr(this->ip.c_str());
    SvrAddr.sin_port = htons(this->port);
    if (bind(this->active_socket, (struct sockaddr*)&SvrAddr,
        sizeof(SvrAddr)) == SOCKET_ERROR) {
        std::cout << "Could not bind socket to address" << std::endl;
        return false;
    }
    else {
        return true;
    }
}

int Server_UDP::send_message(std::string message, std::string ip, int port) {
    struct sockaddr_in SvrAddr;
    SvrAddr.sin_family = AF_INET;
    SvrAddr.sin_port = htons(port);
    SvrAddr.sin_addr.s_addr = inet_addr(ip.c_str());
    return sendto(this->active_socket, message.c_str(), message.length(), 0,
        (struct sockaddr*)&SvrAddr, sizeof(SvrAddr));
}

struct sockaddr_in Server_UDP::receive_message(std::string& message) {
    struct sockaddr_in SvrAddr;
    int addr_len = sizeof(SvrAddr);

    char RxBuffer[128] = {};
    recvfrom(this->active_socket, RxBuffer, sizeof(RxBuffer), 0,
        (struct sockaddr*)&SvrAddr, &addr_len);
    message = RxBuffer;
    return SvrAddr;
}
