#pragma once
#include <string>
#include <iostream>
#include <windows.networking.sockets.h>

#define MAX_BUFFER_SIZE 512
#define MAX_SOCKETS 20

struct packet {
    char name[16];
    bool flag; 
    int size;
    char* letters = nullptr;
};

struct serialized_packet {
    char* data = nullptr;
    int length;
};

struct data_packet {
    char name[16];
    int id;
    int size;
    int* numbers = nullptr;
    int tail;
};


packet create_packet();
serialized_packet packet_serializer(packet);
packet packet_deserializer(char*);

data_packet create_data_packet();
serialized_packet data_packet_serializer(data_packet);
data_packet data_packet_deserializer(char*);

class Node {
public:
    static bool dlls_started;
    static int num_nodes;
    std::string ip;
    std::string role;
    std::string protocol;
    int port;
    SOCKET active_socket;
public:
    Node();
    Node(std::string ip, int port);
    ~Node();
    void start_dlls() const;
    bool create_socket();
    void display_info() const;
};

class Server_TCP : public Node {
private:
    SOCKET aux_socket;
    SOCKET client_sockets[MAX_SOCKETS + 1] = { INVALID_SOCKET };
    bool active_sockets[MAX_SOCKETS + 1] = { false };
public:
    Server_TCP();
    Server_TCP(std::string ip, int port);
    ~Server_TCP();
    bool bind_socket();
    bool listen_for_connections();
    int accept_connection();
    int find_available_socket();
    void close_connection(int socket);

    int send_message(std::string message, int socket);
    int send_packet(packet message, int socket);
    int send_data_packet(data_packet message, int socket);
    int receive_message(std::string& message, int socket);
    int receive_packet(packet& packet, int socket);
    int receive_data_packet(data_packet& data_packet, int socket);
    void broadcast_message(std::string message, int current_socket);
    void broadcast_packet(packet packet, int current_socket);
    void broadcast_data_packet(data_packet data_packet, int current_socket);

    void client_thread(int socket);
};

class Client_TCP : public Node {
public:
    Client_TCP();
    bool connect_socket(std::string ip, int port);
    int send_message(std::string message);
    int send_packet(packet message);
    int send_data_packet(data_packet message);
    int receive_message(std::string& message);
    int receive_packet(packet& packet);
    int receive_data_packet(data_packet& data_packet);
};

class Client_UDP : public Node {
public:
    Client_UDP();
    int send_message(std::string message, std::string ip, int port);
    struct sockaddr_in receive_message(std::string& message);
};

class Server_UDP : public Node {
public:
    Server_UDP();
    Server_UDP(std::string ip, int port);
    bool bind_socket();
    int send_message(std::string message, std::string ip, int port);
    struct sockaddr_in receive_message(std::string& message);
};
