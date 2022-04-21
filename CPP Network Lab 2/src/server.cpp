#include <iostream>
#include <windows.networking.sockets.h>
#pragma comment(lib, "Ws2_32.lib")

#include "connection.h"

int main()
{
    sdds::Connection conn;
	while (true) {
		if (conn.accept_Connection()) {
			conn.online() = true;
			while (conn.online()) {
				char RxBuffer[128] = {};

				if (recv(conn.client_Socket(), RxBuffer, sizeof(RxBuffer), 0) == SOCKET_ERROR)
				{
					std::cout << "Session Closed\n";
					std::cout << "Ready for new connection...\n";
					conn.close_Client();
					break;
				}
				std::cout << "Msg Rx: " << RxBuffer << std::endl;

				char TxBuffer[128] = "Thanks for your message!";
				send(conn.client_Socket(), TxBuffer, sizeof(TxBuffer), 0);
			}
		}
	}
    return 0;
}
