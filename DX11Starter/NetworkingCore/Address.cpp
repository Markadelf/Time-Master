#include "Address.h"

Address::Address()
{

}

Address::Address(sockaddr_in sockAddress)
{
	this->sockAddress = sockAddress;
}

Address::Address(unsigned int a, unsigned int b, unsigned int c, unsigned int d, unsigned short port)
{
	unsigned int address = (a << 24) |
		(b << 16) |
		(c << 8) |
		d;

	sockAddress.sin_family = AF_INET;
	sockAddress.sin_addr.s_addr = htonl(address);
	sockAddress.sin_port = htons(port);
}

short Address::GetPort()
{
	return ntohs(sockAddress.sin_port);
}

unsigned int Address::GetAddress()
{
	return ntohl(sockAddress.sin_addr.s_addr);
}

sockaddr_in* Address::GetSockAddress()
{
	return &sockAddress;
}
