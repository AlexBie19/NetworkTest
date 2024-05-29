//#include "odin.h"
//
//#include <math.h>
//#include <stdio.h>
//#include <winsock2.h>
//#include <winsock.h>
//
//
//#define PORT 12345
//#define SOCKET_BUFFER_SIZE 1024
//#define MAX_CLIENTS 4
//
//const float32 TURN_SPEED = 1.0f;	// how fast player turns
//const float32 ACCELERATION = 20.0f;
//const float32 MAX_SPEED = 200.0f;
//const uint32 TICKS_PER_SECOND = 60;
//const float32 SECONDS_PER_TICK = 1.0f / (float32)TICKS_PER_SECOND;
//
//
//enum class Client_Message : uint8
//{
//	Join,
//	Leave,
//	Input,
//};
//
//enum class Server_Message : uint8
//{
//	Join_Result,
//	State,
//};
//
//struct IP_Endpoint
//{
//	uint32 address;
//	uint16 port;
//};
//
//bool operator==(const IP_Endpoint& a, const IP_Endpoint& b)
//{
//	return a.address == b.address && a.port == b.port;
//}
//
//struct Player_State
//{
//	float32 x, y, facing, speed;
//};
//
//struct Player_Input
//{
//	bool32 up, down, left, right;
//};
//
//static float32 time_since(LARGE_INTEGER t, LARGE_INTEGER frequency)
//{
//	LARGE_INTEGER now;
//	QueryPerformanceCounter(&now);
//	return (float32)(now.QuadPart - t.QuadPart) / (float32)frequency.QuadPart;
//}
//
//int main()
//{
//	WORD winsock_version = 0x202;
//	WSADATA winsock_data;
//	if (WSAStartup(winsock_version, &winsock_data))
//	{
//		printf("WSAStartup failed: %d", WSAGetLastError());
//		return 1;
//	}
//
//	int address_family = AF_INET;
//	int type = SOCK_DGRAM;
//	int protocol = IPPROTO_UDP;
//	SOCKET sock = socket(address_family, type, protocol);
//
//	if (sock == INVALID_SOCKET)
//	{
//		printf("socket failed: %d", WSAGetLastError());
//		WSACleanup();
//		return 1;
//	}
//
//	SOCKADDR_IN local_address;
//	local_address.sin_family = AF_INET;
//	local_address.sin_port = htons(PORT);
//	local_address.sin_addr.s_addr = INADDR_ANY;
//	if (bind(sock, (SOCKADDR*)&local_address, sizeof(local_address)) == SOCKET_ERROR)
//	{
//		printf("bind failed: %d", WSAGetLastError());
//		WSACleanup();
//		return 1;
//	}
//	u_long enabled = 1;
//	ioctlsocket(sock, FIONBIO, &enabled);
//
//	UINT sleep_granularity_ms = 1;
//	bool32 sleep_granularity_was_set = timeBeginPeriod(sleep_granularity_ms) == TIMERR_NOERROR;
//
//	LARGE_INTEGER clock_frequency;
//	QueryPerformanceFrequency(&clock_frequency);
//
//	int8 buffer[SOCKET_BUFFER_SIZE];
//	IP_Endpoint client_endpoints[MAX_CLIENTS];
//	float32 time_since_heard_from_client[MAX_CLIENTS];
//	Player_State client_objects[MAX_CLIENTS];
//	Player_Input client_inputs[MAX_CLIENTS];
//
//	for (uint16 i = 0; i < MAX_CLIENTS; i++)
//	{
//		client_endpoints[i].address = 0;
//	}
//
//	bool32 is_running = 1;
//	while (is_running)
//	{
//		LARGE_INTEGER tick_start_time;
//		QueryPerformanceCounter(&tick_start_time);
//
//		while (true)
//		{
//			int flags = 0;
//			SOCKADDR_IN from;
//			int from_size = sizeof(from);
//			int bytes_received = recvfrom(sock, buffer, SOCKET_BUFFER_SIZE, flags, (SOCKADDR*)&from, &from_size);
//
//			if (bytes_received == SOCKET_ERROR)
//			{
//				int error = WSAGetLastError();
//				if (error != WSAEWOULDBLOCK)
//				{
//					printf("recvfrom returned SOCKET_ERROR, WSAGetLastError() %d", error);
//				}
//				break;
//			}
//
//			IP_Endpoint from_endpoint;
//			from_endpoint.address = from.sin_addr.S_un.S_addr;
//			from_endpoint.port = ntohs(from.sin_port);
//		}
//	}
//
//	WSACleanup();
//	return 0;
//}
