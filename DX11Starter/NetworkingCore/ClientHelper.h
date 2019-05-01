#pragma once
#include "MessageHeader.h"
#include "PacketBuffer.h"
#include "SocketWrapper.h"
#include "Connection.h"

class ClientHelper
{
	// Consider removing if refactoring callback system
	static ClientHelper* CLIENTHELPER;

	// Does the heavy lifting for the communication
	Connection m_server;
	
	// Stores incoming data before we know where it goes
	Buffer m_landing;

	// Socket
	SocketWrapper m_socket;

	// Callback functions
	void(*m_activeCallback)(Buffer& data);
	void(*m_clientCallback)(Buffer& data);

	// Properties
	int m_port;

public:
	ClientHelper(int port, Address server);
	~ClientHelper();

	// Allows specification of callback functions
	void SetClientCallBack(void(*callback)(Buffer& data));
	void SetActiveCallBack(void(*callback)(Buffer& data));

	// Prepares the next buffer and returns a pointer to it
	Buffer* GetNextBuffer(MessageType messageType);

	// Sends the next packet to the server
	bool SendToServer();

	// Listens for incoming data
	void Listen();

	void ResetAcks();

private:
	// Internal functions for interfacing with SocketWrapper
	void ListenHelper(Address ad, const void* data, const int size);
	static void ListenHelperStatic(Address ad, const void* data, const int size);
};

