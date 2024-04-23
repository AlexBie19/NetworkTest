#include "CServer.h"

CServer::CServer() : m_isConnected(false), m_isRunning(false)
{
	m_winsockVersion = 0x202;
	m_addressFamily = AF_INET;
	m_type = SOCK_DGRAM;
	m_protocol = IPPROTO_UDP;
	m_flags = 0;
	m_toLength = 0;
	m_bufferLength = 0;
	m_port = 12345;
	m_socketBufferSize = 1024;

	// Initialize Winsock
	m_socket = socket(m_addressFamily, m_type, m_protocol);
	if (m_socket == INVALID_SOCKET)
	{
		printf("socket failed: %d", WSAGetLastError());
		WSACleanup();
	}
}

CServer::~CServer()
{
	// Close socket and cleanup Winsock
	closeServer();
}

bool CServer::createServer(const CAddress& address)
{
	// Create Server and bind socket to an Address
	if (bind(m_socket, (SOCKADDR*)&address, sizeof(address)) == SOCKET_ERROR)
	{
		printf("bind failed: %d", WSAGetLastError());
		WSACleanup();
		return false;
	}
	m_isRunning = true;
	return true;
}

bool CServer::acceptClient(CAddress& clientAddress)
{
	// Fügt den Client zu einer Liste der verbundenen Clients hinzu
	// und speichert die Adresse des Clients
	//m_client_addresses[clientAddress.getClientID()] = clientAddress.getAddress();

	return true;
}

bool CServer::sendPacket(const CPackets& packet)
{
	/*Sendet ein Packet an alle verbundenen Clients
	for (auto& client : m_client_addresses)
	{
		if (sendto(m_socket, packet.getBuffer(), packet.getBufferLength(), packet.getFlags(), (SOCKADDR*)&client.second, sizeof(client.second)) == SOCKET_ERROR)
		{
			printf("sendto failed: %d", WSAGetLastError());
			WSACleanup();
			return false;
		}
	}
	*/
	return false;
}

int CServer::receivePacket(CPackets& packet)
{
	// Empfängt ein Packet von einem Client
	int flags = 0;
	SOCKADDR_IN from;
	int fromSize = sizeof(from);
	int bytesReceived = recvfrom(m_socket, packet.getBuffer(), m_socketBufferSize, flags, (SOCKADDR*)&from, &fromSize);
	if (bytesReceived == SOCKET_ERROR)
	{
		printf("recvfrom returned SOCKET_ERROR, WSAGetLastError() %d", WSAGetLastError());
		return -1;
	}

	return bytesReceived;
}

void CServer::closeServer()
{
	closesocket(m_socket);
	WSACleanup();
}

bool CServer::isConnected() const
{
	if (m_isConnected)
		return m_isConnected;

	return false;
}

bool CServer::isRunning() const
{
	if (m_isRunning)
		return m_isRunning;
	return false;
}
