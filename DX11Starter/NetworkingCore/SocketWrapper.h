#pragma once
#include <WS2tcpip.h>
#include "Address.h"

class SocketWrapper
{
	int socketHandle;
public:
	SocketWrapper(unsigned short port);
	~SocketWrapper();
	
	void Send(Address outAddress, const void* packet_data, const int packet_size);

	void Recieve(void (*callback)(Address inAddress, const void* packet_data, const int packet_size));

	static bool StartUp();
	static void CleanUp();
};

