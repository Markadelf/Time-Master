#include "ClientHelper.h"

ClientHelper* ClientHelper::CLIENTHELPER;

ClientHelper::ClientHelper(int port, Address server)
{
	// Properties
	ConnectionInfo remote;
	remote.m_address = server;
	m_server.Init(remote);
	m_port = port;

	SocketWrapper::StartUp();

	m_socket.Rebind(port);

    CLIENTHELPER = this;
}

ClientHelper::~ClientHelper()
{
	SocketWrapper::CleanUp();
}

void ClientHelper::SetClientCallBack(void(*callback)(Buffer &data))
{
	m_clientCallback = callback;
}

void ClientHelper::SetActiveCallBack(void(*callback)(Buffer &data))
{
	m_activeCallback = callback;
}

Buffer* ClientHelper::GetNextBuffer(MessageType messageType)
{
	return m_server.GetNextBuffer(messageType);
}

bool ClientHelper::SendToServer()
{
	return m_server.Send(m_socket);
}

void ClientHelper::Listen()
{
	m_socket.Recieve(ListenHelperStatic);
}

void ClientHelper::ResetAcks()
{
	m_server.ResetAcks();
}

bool ClientHelper::CheckDisconnected()
{
	return m_server.CheckDisconnected();
}

void ClientHelper::ListenHelper(Address ad, const void* data, const int size)
{
	// Write to the landing buffer
	m_landing.ResetIndex();
	m_landing.WriteBytes(data, size);
	m_landing.ResetIndex();

	// Read the header from the landing buffer
	MessageHeader head;
	Serializer::Deserialize<MessageHeader>(m_landing, &head);
	switch (head.m_type)
	{
	case MessageType::GameData:
		// Ack and process the data
		m_server.AckAndProcess(m_socket, m_landing, m_activeCallback);
		break;
	case MessageType::GameRequest:
		// Pass the request to the game data
		m_clientCallback(m_landing);
		break;
	case MessageType::ServerRequest:
		// Handle the request here
		// Currently a placeholder
		break;
	case MessageType::Ping:
	default:
		break;
	}
}

void ClientHelper::ListenHelperStatic(Address ad, const void* data, const int size)
{
    CLIENTHELPER->ListenHelper(ad, data, size);
}
