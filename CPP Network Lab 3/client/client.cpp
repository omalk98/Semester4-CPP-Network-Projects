#include <iostream>
#include <string>
#include <thread>
#include <windows.networking.sockets.h>
#pragma comment(lib, "Ws2_32.lib")

// Forward declerations
void getMessages(char*, SOCKET, size_t);
void sendMessages(SOCKET);

int main()
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cout << "Could not start DLLs" << std::endl;
        return 0;
    }

    SOCKET ClientSocket;
    ClientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (ClientSocket == INVALID_SOCKET) {
        std::cout << "Could not create socket" << std::endl;
        WSACleanup();
        return 0;
    }

    struct sockaddr_in SvrAddr;
    SvrAddr.sin_family = AF_INET;
    SvrAddr.sin_port = htons(27000);
    SvrAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    //SvrAddr.sin_addr.s_addr = inet_addr("159.203.26.94");
    if ((connect(ClientSocket, (struct sockaddr*)&SvrAddr,
                    sizeof(SvrAddr))) == SOCKET_ERROR) {
        std::cout << "Failed to connect to server" << std::endl;
        closesocket(ClientSocket);
        WSACleanup();
        return 0;
    }

    char RxBuffer[128] = {};
    recv(ClientSocket, RxBuffer, sizeof(RxBuffer), 0);
    if (!strcmp(RxBuffer, "Full")) {
        std::cout << "Server full" << std::endl;
        return 0;
    }
    else {
        // Display Welcome message
        std::cout << RxBuffer << std::endl;

        // Create detatched thread
        std::thread(getMessages, RxBuffer, ClientSocket, sizeof(RxBuffer)).detach();
        // Create messeging thread, terminates thread when user exits
        std::thread c_send(sendMessages, ClientSocket);

        c_send.join();
    }

    closesocket(ClientSocket);
    WSACleanup();

    return 0;
}


// Thread for sending messages
void sendMessages(SOCKET ClientSocket) {
    std::string TxBuffer;
    while (true) {
        std::cout << "Enter a string ([q]):" << std::endl;
        std::getline(std::cin, TxBuffer);
        send(ClientSocket, TxBuffer.c_str(), TxBuffer.length(), 0);
        if (TxBuffer == "[q]")
            break;
    }
}

// Thread for receiving messages from other users sent by the server
void getMessages(char* RxBuffer, SOCKET ClientSocket, size_t msg_length) {
    while (true) {
        memset(RxBuffer, 0, msg_length);
        recv(ClientSocket, RxBuffer, msg_length, 0);
        if(std::string(RxBuffer) != "")
            std::cout << "user says: " << RxBuffer << std::endl;
    }
}