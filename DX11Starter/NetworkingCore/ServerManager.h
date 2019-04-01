#pragma once
#include "PacketBuffer.h"
#include "Address.h"
#include "SocketWrapper.h"
#include "MessageHeader.h"
#include <unordered_map>
#include "Connection.h"
class ServerManager
{
	// Prolly a better way to do this, may consider refactoring
	static ServerManager* manager;

	// Hash of Users
	// TODO: Consider removal or analysis of std library hash table
	// TODO: Stop hashing by the port
	std::unordered_map<int, int> m_clientMap;

	// User data and packet data
	// Users are first class citizens that use acks to send game data
	Connection* m_activeUserBuffer;

	// Anyone that contacts the server is automatically considered a client
	// Clients can make server request to the game or the server API, but cannot send game data
	int* m_activeUsers;
	ConnectionInfo* m_clients;

	// Client Buffers
	Buffer* m_clientInBuffer;
	Buffer* m_clientOutBuffer;

	// Socket
	SocketWrapper m_socket;

	// Callback functions
	void (*m_activeCallback)(Buffer& data, int playerId);
	void (*m_clientCallback)(Buffer& data, int clientId);

	// Maximums
	int m_maxActiveUsers;
	int m_maxClients;

	// Count connections
	int m_activeCount;
	int m_clientCount;

	// Properties
	int m_port;


public:
	ServerManager(int port, int maxClients, int maxActiveUsers);

	~ServerManager();

	// Allows setting of a delegate used to send data to the game code
	void SetClientCallBack(void (*callback)(Buffer& data, int clientId));
	void SetActiveCallBack(void (*callback)(Buffer& data, int playerId));

	// Prepares the next outward buffer for writing/transmission and returns a pointer to it
	Buffer* GetNextBufferClient(MessageType messageType, int clientId);
	Buffer* GetNextBufferActiveUser(MessageType messageType, int userId);

	// Sends the next packet to a connected user
	bool SendToActiveUser(int userId);

	// Sends the next packet to a client
	bool SendToClient(int clientId);

	// Code begins to treat the client as a user with the given Id
	void SetUser(int clientId, int userId);

	// Checks if any new data is present
	void Listen();

private:
	// Internal functions for interfacing with SocketWrapper
	void ListenHelper(Address ad, const void* data, const int size);
	static void ListenHelperStatic(Address ad, const void* data, const int size);

	// Gets the client id from an address. If the address is new, creates a client id
	int GetClientId(Address ad);

};

