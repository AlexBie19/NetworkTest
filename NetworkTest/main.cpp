#include "Network.h"



int main()
{
	Network net;
	int ret, ret1;
	DataPacket searchPacket;
	searchPacket.type = DataPacketType::Search;
	searchPacket.data[0] = 'H';
	searchPacket.message = "Search for game";
	searchPacket.playerID = net.generatePlayerID();

	DataPacket responsePacket;
	responsePacket.type = DataPacketType::Join;

	net.Init();

	for (int i = 0; i < 10; i++)
	{
		printf("%d\n\n",net.getConnectedDevices());
		ret1 = net.Send(&searchPacket);

		if (ret1 == SOCKET_ERROR)
		{
			printf("sendto returned SOCKET_ERROR, WSAGetLastError() %d", WSAGetLastError());
			break;
		}
		else if (ret1 == 0)
		{
			printf("sendto returned 0\n");
		}
		else
		{
			printf("Sent: %s\n, %d\n PlayerID: %s\n", searchPacket.message.c_str(), searchPacket.type, searchPacket.playerID.c_str());
		}

		ret = net.Receive(&searchPacket, sizeof(searchPacket));

		if (ret == SOCKET_ERROR)
		{
			printf("recvfrom returned SOCKET_ERROR, WSAGetLastError() %d", WSAGetLastError());
			break;
		}
		else if (ret == 0)
		{
			printf("recvfrom returned 0\n");
		}
		else
		{
			printf("Received: %s\n, %d\n PlayerID: %s\n", searchPacket.message.c_str(), searchPacket.type, searchPacket.playerID.c_str());
		}
		getchar();
	}

}