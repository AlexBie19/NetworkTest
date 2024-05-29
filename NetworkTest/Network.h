#pragma once
#include "pch.h"
#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <iphlpapi.h>

#pragma comment (lib, "Ws2_32.lib")
#pragma comment(lib, "IPHLPAPI.lib")

#include <iostream>
#include <string>
#include <stdio.h>
#include <map>
#include <math.h>

#define NETWORK_SEND(net, packet) net.Send(packet, sizeof(packet))


	enum DataPacketType {
		Join = 11,
		Search = 22,
		CarPos = 100,
	};

	struct SearchPacket {
		int gameID = 69;
		int playerID;
		std::string message;
	};

	struct responsePacket {
		int gameID = this->gameID;
		int playerID;
		std::string responseMessage;
	};

	struct PacketCarPos {
		int positionx;
		int positiony;
		int positionz;
	};

	template <typename T>
	struct DataPacketT
	{
		int type;
		int playerFrom;

		union {
			T data;
			//char binaryData[500];
		};

	};

	struct DataPacket
	{
		int type;
		int playerFrom;
		std::string message;
		std::string playerID;

		union {
			char data[500];
		};

	};

	struct NetworkBroadcastDevice
	{
		sockaddr_in m_listenAddress{};
		sockaddr_in m_broadcastAddress{};
		std::string playerID;
	};

	class Network
	{
	public:
		Network();

		void Init();

		void Fini();

		int Receive(DataPacket* packet, int packetSize);

		template <typename T>
		int Send(DataPacketT<T>* p);

		int Send(DataPacket* p);

		int Send(SearchPacket* p);

		int Send(responsePacket* p);

		int getGameID();

		bool Connect(const std::string& address); // Connect to another player

		void Disconnect(); // Disconnect from another player

		bool isConnected(); // Check if connected to another player

		std::string GetPlayerID(); // Get the player ID

		void AddPlayer(const std::string& playerID, const sockaddr_in& address); // Add a player to the list of devices

		void RemovePlayer(const std::string& playerID); // Remove a player from the list of devices

		void BroadcastMessage(std::string message); // Broadcast a message to all devices

		void SearchPlayers(); // Search for other players

		int getConnectedDevices();

		std::string generatePlayerID(); // Generate a unique player ID

	private:

		SOCKET m_socket{};
		NetworkBroadcastDevice m_devices[20];
		sockaddr_in m_peerAdress;
		std::string m_playerID; // Unique ID for this player
		int m_devicesCount;
		bool m_isConnected;
		std::map < std::string, sockaddr_in> m_players;
		int gameID = 69;

	};




	template<typename T>
	inline int Network::Send(DataPacketT<T>* p)
	{

		sockaddr_in broadcastAddress = { AF_INET, htons(21000) };
		inet_pton(AF_INET, "255.255.255.255", &broadcastAddress.sin_addr.s_addr);

		int ret = sendto(m_socket,
			(char*)p,
			sizeof(*p),
			0,
			reinterpret_cast<SOCKADDR*>(&broadcastAddress),
			sizeof(broadcastAddress));

		if (ret == -1) {
			int err = WSAGetLastError();
		}
		return ret;
	}

	inline int Network::Send(SearchPacket* p)
	{

		sockaddr_in broadcastAddress = { AF_INET, htons(21000) };
		inet_pton(AF_INET, "255.255.255.255", &broadcastAddress.sin_addr.s_addr);

		int ret = sendto(m_socket,
			(char*)p,
			sizeof(*p),
			0,
			reinterpret_cast<SOCKADDR*>(&broadcastAddress),
			sizeof(broadcastAddress));

		if (ret == -1) {
			int err = WSAGetLastError();
		}
		return ret;
	}

	inline int Network::Send(responsePacket* p)
	{

		sockaddr_in broadcastAddress = { AF_INET, htons(21000) };
		inet_pton(AF_INET, "255.255.255.255", &broadcastAddress.sin_addr.s_addr);

		int ret = sendto(m_socket,
			(char*)p,
			sizeof(*p),
			0,
			reinterpret_cast<SOCKADDR*>(&broadcastAddress),
			sizeof(broadcastAddress));

		if (ret == -1) {
			int err = WSAGetLastError();
		}
		return ret;
	}

	inline int Network::Send(DataPacket* p)
	{

		sockaddr_in broadcastAddress = { AF_INET, htons(21000) };
		inet_pton(AF_INET, "255.255.255.255", &broadcastAddress.sin_addr.s_addr);

		int ret = sendto(m_socket,
			(char*)p,
			sizeof(*p),
			0,
			reinterpret_cast<SOCKADDR*>(&broadcastAddress),
			sizeof(broadcastAddress));

		if (ret == -1) {
			int err = WSAGetLastError();
		}
		return ret;
	}

