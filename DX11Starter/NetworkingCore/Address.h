#pragma once
#include <WS2tcpip.h>
#include <winsock2.h>
#pragma comment( lib, "wsock32.lib" )

class Address
{
	sockaddr_in sockAddress;

public:
	Address(sockaddr_in sockAddress);
	Address(unsigned int a, unsigned int b, unsigned int c, unsigned int d, unsigned short port);

	short GetPort();
	unsigned int GetAddress();

	sockaddr_in* GetSockAddress();
};

