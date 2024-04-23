#pragma once
#include <winsock2.h>
#include <iostream>
#include <stdio.h>
#include "CSockets.h"
#include "CAddress.h"
#include "CPackets.h"
#include "Odin.h"

#define SOCKET_BUFFER_SIZE 1024

class Client {
public:
		Client();
		~Client();

		void Run();

		bool connectToServer(const CAddress& address);

		void disconnectFromServer();

		bool sendPacket(const CPackets& packet);

		bool receivePacket(CPackets& packet);

		bool isConnected() const;

		void updatePlayerPosition(int x, int y);

		void predictPlayerPosition();

		bool checkCollision();

		void calculateCollsion();

		void closeClient();

private:
		SOCKET m_socket;
		CAddress m_serverAddress;
		bool m_isConnected;
		bool m_isRunning;
		CPackets m_packet;
		WSADATA m_winsockData;
		WORD m_winsockVersion;
		int m_addressFamily;
		int m_type;
		int m_protocol;
		int m_flags;
		int m_toLength;
		int m_bufferLength;
		int m_port;
		int m_socketBufferSize;
		char m_buffer[SOCKET_BUFFER_SIZE];
};