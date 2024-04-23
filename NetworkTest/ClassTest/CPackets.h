#pragma once
#include <winsock2.h>
#include <iostream>
#include <stdio.h>

#include "CSockets.h"
#include "CAddress.h"
#include "CPackets.h"
#include "CClient.h"

class CPackets
{
	public:
	CPackets();

	~CPackets();

	bool createPacket(const char* buffer, int bufferLength, int flags, const sockaddr* to, int toLength);

	bool sendPacket(const char* buffer, int bufferLength, int flags, const sockaddr* to, int toLength);

	int receivePacket(char* buffer, int bufferLength, int flags, sockaddr* from, int* fromSize);

	// Get Buffer
	char* getBuffer(); const

	// Get Buffer Length
	int getBufferLength(); const

	// Get Flags
	int getFlags(); const

	// Get To Length
	int getToLength();

	// Get Socket Buffer Size
	int getSocketBufferSize();

	// Get From Size
	int getFromSize();


	struct playerData {
		CAddress address;
		int id;
		int x;
		int y;
	};
	
	struct playerCar {
		int carID;
		int collision;
	};

	void closePacket();

private:
		char m_buffer[SOCKET_BUFFER_SIZE];
		int m_bufferLength;
		int m_flags;
		int m_toLength;
		int m_socketBufferSize;
		int m_fromSize;
		int m_addressFamily;
		int m_type;
		int m_protocol;
		int m_port;
		SOCKET m_socket;
		sockaddr m_to;
		sockaddr m_from;
		WSADATA m_winsockData;
		WORD m_winsockVersion;
};

