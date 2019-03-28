#pragma once
#include "PacketBuffer.h"
#include "Address.h"
#include "SocketWrapper.h"

template<int maxActiveUsers, int maxClients>
class ServerManager
{
	struct ClientInfo {
		Address m_address;
		int m_activeBuffer = -1;
	};

	// User data and packet data
	PacketBuffer m_outBuffer[maxActiveUsers];
	PacketBuffer m_inBuffer[maxActiveUsers];
	// Client ID is an index
	int m_activeUsers[maxActiveUsers];
	UserInfo m_clients[maxClients];

	// Socket
	SocketWrapper m_socket;

	// Callback functions
	void (*m_activeCallback)(Buffer& data);
	void (*m_clientCallback)(Buffer& data);

	// Count connections
	int m_activeCount;
	int m_clientCount;

	// Properties
	int m_port;


public:
	NetworkManager(int port) {
		m_activeCount = 0;
		m_clientCount = 0;
		m_port = port;

		SocketWrapper::StartUp();
		m_socket = SocketWrapper(m_port);
	}

	~NetworkManager() {
		SocketWrapper::CleanUp();
	}

	bool SendToActiveUser(int userId, Buffer& message){

	}

	bool SendToClient(int clientId, Buffer& message){
		m_socket.Send(m_clients[clientId], message.GetBuffer(), message.Size())
	}

	int AddClient(ClientInfo client) {
		if (maxClients > m_clientCount) {
			m_clients[m_clientCount] = client;
			return m_clientCount++;
		}
		return -1;
	}

	void Listen() {
		m_socket.Recieve(&ListenHelper);
	}

private:
	ListenHelper(Address ad, const void* data, const int size) {
	
	}

};

