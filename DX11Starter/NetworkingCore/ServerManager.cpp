#include "ServerManager.h"

ServerManager* ServerManager::manager;

ServerManager::ServerManager(int port, int maxClients, int maxActiveUsers) {
	manager = this;

	m_activeCount = 0;
	m_clientCount = 0;
	m_port = port;

	m_maxActiveUsers = maxActiveUsers;
	m_maxClients = maxClients;

	m_activeUsers = new int[maxActiveUsers];
	m_clients = new ConnectionInfo[maxClients];
	m_activeUserBuffer = new Connection[maxActiveUsers];
	m_clientInBuffer = new Buffer[maxClients];
	m_clientOutBuffer = new Buffer[maxClients];

	SocketWrapper::StartUp();
	m_socket.Rebind(port);

	for (size_t i = 0; i < maxActiveUsers; i++)
	{
		m_activeUsers[i] = -1;
	}
}

ServerManager::~ServerManager() {

	delete[] m_activeUsers;
	delete[] m_clients;
	delete[] m_activeUserBuffer;
	delete[] m_clientInBuffer;
	delete[] m_clientOutBuffer;

	SocketWrapper::CleanUp();
}

void ServerManager::SetClientCallBack(void(*callback)(Buffer &data, int clientId))
{
	m_clientCallback = callback;
}

void ServerManager::SetActiveCallBack(void(*callback)(Buffer &data, int playerId))
{
	m_activeCallback = callback;
}

Buffer* ServerManager::GetNextBufferClient(MessageType messageType, int clientId)
{
	Buffer* buffer = &m_clientOutBuffer[clientId];
	buffer->ResetIndex();
	MessageHeader header;
	header.m_type = messageType;
	header.Serialize(*buffer);
	return buffer;
}

Buffer* ServerManager::GetNextBufferActiveUser(MessageType messageType, int userId)
{
	return m_activeUserBuffer[userId].GetNextBuffer(messageType);
}

bool ServerManager::SendToActiveUser(int userId) {
	return m_activeUserBuffer[userId].Send(m_socket);
}

bool ServerManager::SendToClient(int clientId) {
	m_socket.Send(m_clients[clientId].m_address, m_clientOutBuffer[clientId].GetBuffer(), (int)m_clientOutBuffer[clientId].Size());
	m_clients[clientId].m_nextSend++;
	return true;
}

void ServerManager::SetUser(int clientId, int userId)
{
	// Unset old user
	if (m_activeUsers[userId] != -1)
	{
		m_clients[m_activeUsers[userId]].m_activeBuffer = -1;
	}
	
	// Set new user
	m_activeUsers[userId] = clientId;
	m_clients[clientId].m_activeBuffer = userId;

	// Reinitialize the connection
	m_activeUserBuffer[userId].Init(m_clients[clientId]);
}

void ServerManager::ClearUsers()
{
	for (int i = 0; i < m_maxActiveUsers; i++)
	{
		if (m_activeUsers[i] != -1)
		{
			m_clients[m_activeUsers[i]].m_activeBuffer = -1;
			m_activeUsers[i] = -1;
		}
	}
}

void ServerManager::Listen() {
	m_socket.Recieve(ListenHelperStatic);
}

void ServerManager::ListenHelper(Address ad, const void* data, const int size) {
	// Figure out which client this is
	int clientId = GetClientId(ad);
	
	// Grab the landing buffer for this client
	Buffer* clientBuffer = &m_clientInBuffer[clientId];
	clientBuffer->ResetIndex();
	clientBuffer->WriteBytes(data, size);
	clientBuffer->ResetIndex();
	
	// Grab the message header
	MessageHeader head;
	Serializer::Deserialize<MessageHeader>(*clientBuffer, &head);
	switch (head.m_type)
	{
	case MessageType::GameData:
	{
		// Check if this client has the right permissions
		int userId = m_clients[clientId].m_activeBuffer;
		if (userId != -1)
		{
			// Ack and process the packet
			m_activeUserBuffer[userId].AckAndProcess(m_socket, *clientBuffer, m_activeCallback);
		}
		break;
	}
	case MessageType::GameRequest:
		// Pass the rest of the message to the game code
		m_clientCallback(*clientBuffer, clientId);
		break;
	case MessageType::ServerRequest:
		// Currently a placeholder
		break;
	case MessageType::Ping:
		// Pinging is a good way to get something on the server's radar
	default:
		break;
	}
}

void ServerManager::ListenHelperStatic(Address ad, const void* data, const int size)
{
	// Calls the nonstatic version
	manager->ListenHelper(ad, data, size);
}

int ServerManager::GetClientId(Address ad)
{
	// Currently users are only differentiated by the address they are on
	auto client = m_clientMap.find(ad.GetAddress());
	int clientId = -1;
	if (client == m_clientMap.end())
	{
		ConnectionInfo info;
		info.m_address = ad;
		// Sanity check if we ran out of slots
		if (m_maxClients > m_clientCount) {
			m_clients[m_clientCount] = info;
			clientId = m_clientCount++;
		}
		else
		{
			return -1;
		}
		m_clientMap[ad.GetAddress()] = clientId;
	}
	else
	{
		clientId = client->second;
	}
	return clientId;
}
