#pragma once
#include <winsock2.h>
#include <iostream>
#include <stdio.h>

class CSockets
{
public:
	CSockets();
	~CSockets();

	bool createSocket(int addressFamily, int type, int protocol);

	bool bindSocket(int port);

	bool sendTo(const char* buffer, int bufferLength, int flags, const sockaddr* to, int toLength);

	int receiveFrom(char* buffer, int bufferLength, int flags, sockaddr* from, int* fromSize);

	void closeSocket();

private:
		SOCKET m_socket;
		int m_addressFamily;
		int m_type;
		int m_protocol;
		int m_port;
		int m_socketBufferSize;
		int m_flags;
		int m_toLength;



};

