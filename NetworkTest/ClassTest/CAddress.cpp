#include "CAddress.h"

CAddress::CAddress()
{
	address_family = AF_INET;
	type = SOCK_DGRAM;
	protocol = IPPROTO_UDP;
	sock = socket(address_family, type, protocol);

	if (sock == INVALID_SOCKET)
	{
		printf("socket failed: %d", WSAGetLastError());
		WSACleanup();
	}

	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(PORT);
	server_address.sin_addr.S_un.S_un_b.s_b1 = 127;
	server_address.sin_addr.S_un.S_un_b.s_b2 = 0;
	server_address.sin_addr.S_un.S_un_b.s_b3 = 0;
	server_address.sin_addr.S_un.S_un_b.s_b4 = 1;

	m_port = PORT;
}

CAddress::CAddress(const char* ip, int port)
{
	address_family = AF_INET;
	type = SOCK_DGRAM;
	protocol = IPPROTO_UDP;
	sock = socket(address_family, type, protocol);

	if (sock == INVALID_SOCKET)
	{
		printf("socket failed: %d", WSAGetLastError());
		WSACleanup();
	}

	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(port);
	server_address.sin_addr.S_un.S_un_b.s_b1 = 127;
	server_address.sin_addr.S_un.S_un_b.s_b2 = 0;
	server_address.sin_addr.S_un.S_un_b.s_b3 = 0;
	server_address.sin_addr.S_un.S_un_b.s_b4 = 1;

	strcpy_s(m_ip, ip);
	m_port = port;
}

CAddress::~CAddress()
{
	WSACleanup();
}

const char* CAddress::getIP() const
{
	return m_ip;
}

int CAddress::getPort() const
{
	return m_port;
}

void CAddress::setIP(const char* ip)
{
	strcpy_s(m_ip, ip);
}

void CAddress::setPort(int port)
{
	m_port = port;
}

int CAddress::getClientID() const
{
	
	return 0;
}

int CAddress::getAddress() const
{
		return 0;
}
