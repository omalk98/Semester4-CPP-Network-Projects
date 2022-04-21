#include <iostream>
#include <string>
#include <windows.networking.sockets.h>
#pragma comment(lib, "Ws2_32.lib")
#include"connection.h"

namespace sdds 
{
    Connection* Connect();
    void Close_Connection(Connection*);
    bool Transmit_Msg(Connection*);
    bool Reconnect(Connection*, bool = false);
}

void run();

int main()
{   
    run();
    return 0;
}

void run() 
{
    sdds::Connection* conn = sdds::Connect();
    while (sdds::Reconnect(conn, sdds::Transmit_Msg(conn)));
}

namespace sdds 
{
    Connection* Connect()
    {
        Connection* connection = new Connection;
        return connection;
    }

    void Close_Connection(Connection* conn) 
    {
        delete conn;
        conn = nullptr;
    }

    bool Transmit_Msg(Connection* conn)
    {
        bool trmnt = false;
        while (conn->online() && !trmnt) 
        {
            std::string TxBuffer;

            std::cout << "Enter a String to transmit, type [q] at any time to Quit:" << std::endl;
            std::getline(std::cin, TxBuffer);

            if (TxBuffer == "[q]") 
            {
                std::cout << "goodbye";
                trmnt = true;
                break;
            }

            send(conn->client_Socket(), TxBuffer.c_str(), sizeof(*TxBuffer.c_str()) * TxBuffer.size(), 0);

            char RxBuffer[128] = {};
            if (recv(conn->client_Socket(), RxBuffer, sizeof(RxBuffer), 0) == SOCKET_ERROR) 
            {
                std::cout << "ERROR 504: Gateway Timeout.\n";
                conn->online() = false;
                break;
            }
            std::cout << "Response: " << RxBuffer << std::endl;
        }
        return trmnt;
    }

    bool Reconnect(Connection* conn, bool trmnt)
    {
        Close_Connection(conn);
        while (!trmnt) 
        {
            std::string ok;

            std::cout << "Would you like to Reconnect to the server?[Y/N] ";
            std::getline(std::cin, ok);

            if (ok == "y" || ok == "Y")
            {
                conn = Connect();
                break;
            }
            else if (ok == "n" || ok == "N" || ok == "[q]")
            {
                trmnt = true;
                break;
            }
            else
            {
                std::cout << "Invalid Entry.\n";
            }
        }
        return !trmnt;
    }
}