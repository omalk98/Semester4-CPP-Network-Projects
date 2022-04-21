#ifndef SDDS_CONNECTION_H
#define SDDS_CONNECTION_H
#include <windows.networking.sockets.h>
#pragma comment(lib, "Ws2_32.lib")

namespace sdds 
{
	class Connection 
	{
	private:
		WSADATA m_wsaData;
		SOCKET m_ClientSocket;
		sockaddr_in m_SvrAddr;
		bool m_Conn_Online;
		bool init_WinSock();
		bool create_Socket();
		bool bind_Socket(const char*, const unsigned);
		void Cleanup(const bool)const;
		bool setup_Connection(const char*, const unsigned);
	public:
		Connection(const char* = "127.0.0.1", const unsigned = 27000);
		~Connection();
		bool& online();
		SOCKET& client_Socket();
	};
}

#endif // !SDDS_CONNECTION_H
