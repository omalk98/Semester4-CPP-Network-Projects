#include <iostream>
#include <windows.networking.sockets.h>
#include "connection.h"

namespace sdds 
{

	bool Connection::init_WinSock()
	{
		return !WSAStartup(MAKEWORD(2, 2), &m_wsaData);
	}

	bool Connection::create_Socket()
	{
		m_ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		return !(m_ListenSocket == INVALID_SOCKET);
	}

	bool Connection::bind_Socket(const char* ip, const unsigned port)
	{
		m_SvrAddr.sin_family = AF_INET;
		m_SvrAddr.sin_port = htons(port);
		m_SvrAddr.sin_addr.s_addr = inet_addr(ip);
		return !(bind(m_ListenSocket, (sockaddr*)&m_SvrAddr, sizeof(m_SvrAddr)) == SOCKET_ERROR);
	}

	bool Connection::start_listen()
	{
		return !(listen(m_ListenSocket, 1) == SOCKET_ERROR);
	}

	void Connection::Cleanup()const
	{
		closesocket(m_ListenSocket);
		closesocket(m_ClientSocket);
		WSACleanup();
	}

	bool Connection::setup_Listener(const char* ip, const unsigned port)
	{
		if (!create_Socket()) 
		{
			std::cout << "ERROR: Could not create socket\n";
			Cleanup();
			return false;
		}
		if (!bind_Socket(ip, port))
		{
			std::cout << "ERROR: Failed to bind socket to address\n";
			Cleanup();
			return false;
		}
		if (!start_listen()) 
		{
			std::cout << "ERROR: Failed to start listening\n";
			Cleanup();
			return false;
		}
		return true;
	}

	Connection::Connection(const char* ip, const unsigned port)
	{
		if (init_WinSock()) 
			m_Conn_Online = setup_Listener(ip, port);
		else
		{
			std::cout << "ERROR: Could not start DLLs\n";
			Cleanup();
		}
	}

	Connection::~Connection()
	{
		if (m_Conn_Online) 
		{
			Cleanup();
			m_Conn_Online = false;
		}
	}

	bool Connection::accept_Connection()
	{
		std::cout << "Waiting for client connection" << std::endl;

		m_ClientSocket = accept(m_ListenSocket, NULL, NULL);
		if (m_ClientSocket == INVALID_SOCKET) {
			std::cout << "ERROR: Failed to accept connection\n";
			close_Client();
			return false;
		}
		closesocket(m_ListenSocket);
		std::cout << "Connection Established" << std::endl;
		return true;
	}

	void Connection::close_Client()
	{
		closesocket(m_ClientSocket);
		m_Conn_Online = false;
		setup_Listener("127.0.0.1", 27000);
	}

	bool& Connection::online()
	{
		return m_Conn_Online;
	}

	SOCKET& Connection::client_Socket()
	{
		return m_ClientSocket;
	}
}