#pragma once

#include <winsock2.h>
#include <iostream>
#include <stdio.h>
#include <map>

#include "Odin.h"
#include "CSockets.h"
#include "CAddress.h"
#include "CPackets.h"
#include "CClient.h"

#define SOCKET_BUFFER_SIZE 1024
#define MAX_CLIENTS 4
#define CLIENT_TIMEOUT 5.0f
#define PORT 12345

const uint32 TICKS_PER_SECOND = 60;
const float32 SECONDS_PER_TICK = 1.0f / (float32)TICKS_PER_SECOND;

class CServer
{
	CServer();
	~CServer();

	bool createServer(const CAddress& address);

	bool acceptClient(CAddress& clientAddress);

	bool sendPacket(const CPackets& packet);

	int receivePacket(CPackets& packet);

	bool isConnected() const;

	bool isRunning() const;
	
	void updatePlayerPosition(int x, int y);

	void checkCollision();

	void closeServer();

private:
		std::map<int, SOCKADDR_IN> m_client_addresses; // Speichert die Adressen der Clients
		std::map<int, std::pair<int,int>> m_client_positions; // Speichert die Positionen der Clients
		SOCKET m_socket;
		CAddress m_serverAddress;
		bool m_isConnected;
		bool m_isRunning;
		CPackets m_packet;
		WSADATA m_winsockData;
		WORD m_winsockVersion = 0x202;
		int m_addressFamily = AF_INET;
		int m_type = SOCK_DGRAM;
		int m_protocol = IPPROTO_UDP;
		int m_flags;
		int m_toLength;
		int m_bufferLength;
		int m_port = 12345 ;
		int m_socketBufferSize = 1024;
		char m_buffer[SOCKET_BUFFER_SIZE];
};
