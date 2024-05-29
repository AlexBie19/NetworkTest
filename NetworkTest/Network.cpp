#include "Network.h"
#include <random>

	Network::Network()
	{
		m_devicesCount = 0;
	}

	void Network::Init()
	{
		WSAData data;
		if (WSAStartup(MAKEWORD(2, 2), &data) != NULL) {
			throw std::system_error(WSAGetLastError(), std::system_category(), "WSA failed to start");
		};


		unsigned long outBufLen = 15000;
		PIP_ADAPTER_ADDRESSES adapters = (IP_ADAPTER_ADDRESSES*)malloc(outBufLen);
		GetAdaptersAddresses(AF_INET, NULL, NULL, adapters, &outBufLen);

		PIP_ADAPTER_ADDRESSES next = adapters;
		int i = 0;
		while (next != NULL) {

			// 16777343 = 127.0.0.1, loopback wollen wir nicht
			if (((LPSOCKADDR_IN)next->FirstUnicastAddress->Address.lpSockaddr)->sin_addr.S_un.S_addr != 16777343) {

				// Windows hat verschiedene Repräsentationen für Netwerkadressen in der Winapi. 
				// Jedoch sind diese meist zueinander castbar! Eine IN_ADDR ist eine IPv4-Adresse z. B.
				// Während eine LPSOCKADDR eine generische Netzwerkadresse ist die jenseits von IP auch mit IPX, NetBeui usw. bestückbar ist
				// Diese darf ich aber, da ich oben mit AF_INET eh nur ipv4 adressen hole, problemlos zu in_addr casten
				in_addr unicastAddress = ((LPSOCKADDR_IN)next->FirstUnicastAddress->Address.lpSockaddr)->sin_addr;

				// Eine Subnetzmaske darf immer nur eine durchgängige Menge von 1-Bits haben, eine 0 und wieder eine 1 ist hier nicht erlaubt!
				// Daher kann man eine Subnetzmaske auch einfach als Länge angeben. Wir wollen sie aber als Adresse.
				unsigned char subnetLength = next->FirstUnicastAddress->OnLinkPrefixLength;
				in_addr subnetMask = {};
				ConvertLengthToIpv4Mask(subnetLength, &subnetMask.S_un.S_addr);

				// Broadcastadresse = IP-Adresse, bei der alle bits 1 sind, bei der die Subnetzmaske 0 stehen.
				in_addr broadcastAddress = {};
				broadcastAddress.S_un.S_addr = unicastAddress.S_un.S_addr | (~subnetMask.S_un.S_addr);

				// String-Konvertierungen, nur zur Ausgabe!
				char sUnicastAddress[16];
				inet_ntop(AF_INET, &unicastAddress, (char*)&sUnicastAddress, 16);
				char sSubnetMask[16];
				inet_ntop(AF_INET, &subnetMask, (char*)&sSubnetMask, 16);
				char sBroadcastAddress[16];
				inet_ntop(AF_INET, &broadcastAddress, (char*)&sBroadcastAddress, 16);
				char networkAdapterName[20];
				wcstombs_s(NULL, (char*)&networkAdapterName, 20, next->FriendlyName, _TRUNCATE);

				printf("Adapter %s: %s %s %s \r\n", networkAdapterName, sUnicastAddress, sSubnetMask, sBroadcastAddress);

				m_devices[i] = {};
				m_devices[i].m_broadcastAddress.sin_addr = broadcastAddress;
				m_devices[i].m_listenAddress.sin_addr = unicastAddress;
				m_devices[i].playerID = generatePlayerID();

				i++;
				m_devicesCount++;
				printf("Devicecount: %d\n", m_devicesCount);
			}

			next = next->Next;
		}
		free(adapters);


		m_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

		char broadcast = 1;
		int sockopt = setsockopt(m_socket, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast));

		unsigned long blockingMode = -1;
		ioctlsocket(m_socket, FIONBIO, &blockingMode);

		sockaddr_in listenAddress = { AF_INET, htons(21000) };
		inet_pton(AF_INET, "0.0.0.0", &listenAddress.sin_addr.s_addr);
		bind(m_socket, (sockaddr*)&listenAddress, sizeof(listenAddress));
	}

	void Network::Fini()
	{
		closesocket(m_socket);
	}

	int Network::Receive(DataPacket* packet, int packetSize)
	{
		sockaddr_in from;
		int size = sizeof(from);

		WSABUF buffer = {};
		buffer.len = packetSize;
		buffer.buf = (char*)packet;

		int ret = recvfrom(m_socket, (char*)packet, packetSize, 0, reinterpret_cast<SOCKADDR*>(&from), &size);
		if (ret == SOCKET_ERROR) {
			int wsaErr = WSAGetLastError();
			if (wsaErr == WSAEWOULDBLOCK) {
				return 0;
			}
			else {
				throw std::system_error(WSAGetLastError(), std::system_category(), "recvfrom failed");
			}
		}
		return ret;
	}

	int Network::getGameID()
	{
		return gameID;
	}

	bool Network::Connect(const std::string& address)
	{
		m_peerAdress.sin_family = AF_INET;
		m_peerAdress.sin_port = htons(21000);

		if (inet_pton(AF_INET, address.c_str(), &(m_peerAdress.sin_addr)) <= 0)
		{
			return false;		// Address Conversion failed
		}

		if (connect(m_socket, (struct sockaddr*)&m_peerAdress, sizeof(m_peerAdress)) < 0)
		{
			return false;		// Connection failed
		}

		m_isConnected = true;

		m_playerID = generatePlayerID();

		return true;
	}

	void Network::Disconnect()
	{
		closesocket(m_socket);

		m_isConnected = false;
	}

	bool Network::isConnected()
	{
		return m_isConnected;
	}

	std::string Network::GetPlayerID()
	{
		return m_playerID;
	}

	void Network::AddPlayer(const std::string& playerID, const sockaddr_in& address)
	{
		m_players[playerID] = address;
	}

	void Network::RemovePlayer(const std::string& playerID)
	{
		m_players.erase(playerID);
	}

	void Network::BroadcastMessage(std::string message)
	{
		for (const auto& player : m_players)
		{
			sendto(m_socket, message.c_str(), message.size(), 0, (sockaddr*)&player.second, sizeof(player.second));
		}
	}

	void Network::SearchPlayers()
	{
		// Erstelle ein SearchPacket mit der Nachricht "SearchMrMax"
		SearchPacket searchPacket;
		searchPacket.message = "SearchMrMax";

		// Sende das SearchPacket an alle Geräte im Netzwerk
		for (int i = 0; i < m_devicesCount; i++)
		{
			sendto(m_socket, (char*)&searchPacket, sizeof(searchPacket), 0, (sockaddr*)&m_devices[i].m_broadcastAddress, sizeof(m_devices[i].m_broadcastAddress));
		}

		// Initialisiere eine Antwortstruktur
		responsePacket response;
		response.responseMessage = "JoinMrMax";

		// Empfange Antworten von anderen Geräten
		sockaddr_in from;
		int size = sizeof(from);

		while (true)
		{
			int ret = recvfrom(m_socket, (char*)&response, sizeof(response), 0, reinterpret_cast<SOCKADDR*>(&from), &size);
			if (ret == SOCKET_ERROR) {
				int wsaErr = WSAGetLastError();
				if (wsaErr == WSAEWOULDBLOCK) {
					break;
				}
				else {
					throw std::system_error(WSAGetLastError(), std::system_category(), "recvfrom failed");
				}
			}

			// Wenn die Antwort "JoinMrMax" lautet, füge den Spieler hinzu
			if (response.responseMessage == "JoinMrMax")
			{
				AddPlayer(std::to_string(response.playerID), from);
			}
		}
	}

	int Network::getConnectedDevices()
	{
		return m_devicesCount;
	}

	std::string Network::generatePlayerID()
	{
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> dis(1, 9999);

		std::stringstream ss;

		ss << std::hex << dis(gen);

		return ss.str();
	}
