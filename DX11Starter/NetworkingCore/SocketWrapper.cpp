#include "SocketWrapper.h"
#include <iostream>


SocketWrapper::SocketWrapper(unsigned short port)
{
	socketHandle = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(port);

	int b = bind(socketHandle, (const sockaddr*)&address, sizeof(sockaddr_in));

	if (b == SOCKET_ERROR) {
		std::cout << ("failed to bind\n");
	}

	DWORD nonBlocking = 1;
	ioctlsocket(socketHandle, FIONBIO, &nonBlocking);
}


SocketWrapper::~SocketWrapper()
{
	closesocket(socketHandle);
}

void SocketWrapper::Send(Address outAddress, const void* packet_data, const int packet_size)
{
	int sent = sendto(socketHandle, reinterpret_cast<const char*>(packet_data), packet_size, 0, (sockaddr*)outAddress.GetSockAddress(), sizeof(sockaddr_in));
	if (sent < packet_size)
	{
		std::cout << ("failed to send packet\n");
	}
}

void SocketWrapper::Recieve(void(*callback)(Address inAddress, const void* packet_data, const int packet_size))
{
	while (true)
	{
		unsigned char packet_data[256];
		unsigned int max_packet_size = sizeof(packet_data);

		typedef int socklen_t;

		sockaddr_in from;
		socklen_t fromLength = sizeof(from);

		int bytes = recvfrom(socketHandle, (char*)packet_data, max_packet_size,	0, (sockaddr*)&from, &fromLength);

		if (bytes <= 0)
			break;

		unsigned int from_address =
			ntohl(from.sin_addr.s_addr);

		unsigned int from_port =
			ntohs(from.sin_port);

		callback(from, packet_data, bytes);
	}
}

bool SocketWrapper::StartUp()
{
	WSAData wsaData;
	return WSAStartup(MAKEWORD(2, 2), &wsaData);
}

void SocketWrapper::CleanUp()
{
	WSACleanup();
}
