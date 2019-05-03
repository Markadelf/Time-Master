#pragma once
#include <WS2tcpip.h>
#include "Address.h"

// Wraps around the socket to facilitate communication
class SocketWrapper
{
	int socketHandle;
public:
	SocketWrapper();
	SocketWrapper(unsigned short port);
	~SocketWrapper();

	// Closes any current socket and opens a new one on the given port
	void Rebind(unsigned short port);
	
	// Sends data to an address
	void Send(Address outAddress, const void* packet_data, const int packet_size);

	// Attempts to recieve data and pass it to the callback function
	void Recieve(void (*callback)(Address inAddress, const void* packet_data, const int packet_size));

	// Call once while the engine is starting
	static bool StartUp();
	// Call once when the engine is shutting down
	static void CleanUp();
};

