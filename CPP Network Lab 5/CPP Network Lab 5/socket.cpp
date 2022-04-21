#include "socket.h"
#include <string>
#include <iostream>
#include <windows.networking.sockets.h>
#include <stdlib.h>
#pragma comment(lib, "Ws2_32.lib")

bool Node::dlls_started = false;
int Node::num_nodes = 0;

Node::Node() {
    start_dlls();
    num_nodes++;
    this->active_socket = INVALID_SOCKET;
    this->protocol = "tcp";
    this->ip = "127.0.0.1";
    this->port = 27000;    
    std::cout << "Node() was Called\n";
}

Node::Node(std::string ip, int port) {
    start_dlls();
    num_nodes++;
    this->active_socket = INVALID_SOCKET;
    this->ip = ip;
    this->port = port;
    std::cout << "Node(ip : " << ip  << ", port : " << port << ") was Called\n";
}

Node::~Node() {
    closesocket(this->active_socket);
    std::cout << "Closing socket" << std::endl; 
    num_nodes--;
    if (num_nodes == 0) {
        std::cout << "~Node() was Called\n";
        WSACleanup();
    }
}

void Node::start_dlls() const{
    if (!dlls_started) {
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            std::cout << "Could not start DLLs" << std::endl;
            exit(EXIT_FAILURE);
        }
        else {
            std::cout << "start_dlls() was Called\n";
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
        std::cout << "create_socket() was Called\n";
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
    this->client_socket = INVALID_SOCKET;
    std::cout << "Server_TCP() was Called\n";
}

Server_TCP::Server_TCP(std::string ip, int port) : Node(ip, port) {
    this->protocol = "tcp"; 
    this->role = "server";
    this->client_socket = INVALID_SOCKET;
    std::cout << "Server_TCP(ip : " << ip << ", port : " << port << ") was Called\n";
}

Server_TCP::~Server_TCP() {
    closesocket(this->client_socket);
    std::cout << "Closing client socket" << std::endl;
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
        std::cout << "bind_socket() was Called\n";
        return true;
    }
}

bool Server_TCP::listen_for_connections() {
    if (listen(this->active_socket, 1) == SOCKET_ERROR) {
        std::cout << "Could not start to listen" << std::endl;
        return false;
    }
    else {
        std::cout << "listen_for_connections() was Called\n";
        return true;
    }
}

bool Server_TCP::accept_connection() {
    this->client_socket = accept(this->active_socket, NULL, NULL);
    if (this->client_socket == INVALID_SOCKET){
        std::cout << "Failed to accept connection" << std::endl;
        return false;
    }
    else {
        std::cout << "accept_connection() was Called\n";
        return true;
    }
}

int Server_TCP::send_message(std::string message) {
    std::cout << "Server send_message(message : " << message << ") was Called\n";
    return send(this->client_socket, message.c_str(), message.length(), 0);
}

int Server_TCP::receive_message(std::string& message) {
    std::cout << "Server receive_message(message : " << message << ") was Called\n";
    char RxBuffer[MAX_BUFFER_SIZE] = {};
    memset(RxBuffer, 0, MAX_BUFFER_SIZE);
    int num_bytes = recv(this->client_socket, RxBuffer, MAX_BUFFER_SIZE, 0);
    message = RxBuffer;
    return num_bytes;
}

void Server_TCP::close_connection() {
    std::cout << "Server close_connection() was Called\n";
    closesocket(this->client_socket);
}

Client_TCP::Client_TCP() : Node() {
    this->protocol = "tcp"; 
    this->role = "client";
    std::cout << "Client_TCP() was Called\n";
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
        std::cout << "Client connect_socket() was Called\n";
        return true;
    }
}

int Client_TCP::send_message(std::string message) {
    std::cout << "Client send_message(message : " << message << ") was Called\n";
    return send(this->active_socket, message.c_str(), message.length(), 0);
}

int Client_TCP::receive_message(std::string &message) {
    std::cout << "Client receive_message(message : " << message << ") was Called\n";
    char RxBuffer[MAX_BUFFER_SIZE] = {};
    memset(RxBuffer, 0, MAX_BUFFER_SIZE);
    int num_bytes = recv(this->active_socket, RxBuffer, MAX_BUFFER_SIZE, 0);
    message = RxBuffer;
    return num_bytes;
}
