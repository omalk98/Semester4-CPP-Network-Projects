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
		m_ClientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		return !(m_ClientSocket == INVALID_SOCKET);
	}

	bool Connection::bind_Socket(const char* ip, const unsigned port)
	{
		m_SvrAddr.sin_family = AF_INET;
		m_SvrAddr.sin_port = htons(port);
		m_SvrAddr.sin_addr.s_addr = inet_addr(ip);
		return !(connect(m_ClientSocket, (sockaddr*)&m_SvrAddr, sizeof(m_SvrAddr)) == SOCKET_ERROR);
	}

	void Connection::Cleanup(const bool no_sock = false)const
	{
		if (!no_sock) closesocket(m_ClientSocket);
		WSACleanup();
	}

	bool Connection::setup_Connection(const char* ip, const unsigned port)
	{
		if (!init_WinSock())
		{
			std::cout << "ERROR: Could not start DLLs\n";
			Cleanup(true);
			return false;
		}
		if (!create_Socket()) 
		{
			std::cout << "ERROR: Could not create socket\n";
			Cleanup(true);
			return false;
		}
		if (!bind_Socket(ip, port))
		{
			std::cout << "ERROR: Failed to connect to server\n";
			Cleanup();
			return false;
		}
		return true;
	}

	Connection::Connection(const char* ip, const unsigned port)
	{
		m_Conn_Online = setup_Connection(ip, port);
	}

	Connection::~Connection()
	{
		if (m_Conn_Online) 
		{
			Cleanup();
			m_Conn_Online = false;
		}
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