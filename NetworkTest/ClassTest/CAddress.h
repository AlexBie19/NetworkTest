#pragma once
#include "CServer.h"
#include "CPackets.h"
#include "CClient.h"


class CAddress
{
public:
	CAddress();

	CAddress(const char* ip, int port);

	~CAddress();

	const char* getIP() const;

	int getPort() const;

	void setIP(const char* ip);

	void setPort(int port);

	int getClientID() const;

	int getAddress() const;




private:
	    SOCKADDR_IN server_address;
		SOCKET sock;
		int address_family;
		int type;
		int protocol;
		char m_ip[16];
		int m_port;

};

