#include "Connection.h"

Connection::Connection()
{
}

Connection::~Connection()
{
}

void Connection::Init(ConnectionInfo remote)
{
	m_remote = remote;
	m_outBuffer.Reset();
	m_inBuffer.Reset();
}

Buffer* Connection::GetNextBuffer(MessageType messageType)
{
	// Flag as unacked
	PacketData data;
	data.m_acked = false;
	m_outBuffer.SetPacketData(m_remote.m_nextSend, data);

	// Grab the buffer from the packet ring buffer
	Buffer* buffer = m_outBuffer.GetPacketBuffer(m_remote.m_nextSend);
	// Reset the buffer
	buffer->ResetIndex();

	// Serialize the header
	MessageHeader header;
	header.m_type = messageType;
	header.Serialize(*buffer);
	switch (messageType)
	{
	case GameData:
	{
		// Only game data uses acks at this time.
		// Serialize the ack header
		AckHeader* ack = m_inBuffer.GetAckHeader();
		ack->m_packetId = m_remote.m_nextSend;
		Serializer::Serialize<AckHeader>(*buffer, ack);
		break;
	}
	case GameRequest:
	case ServerRequest:
	case Ping:
	default:
		break;
	}
	m_nextType = messageType;
	return buffer;
}

bool Connection::Send(SocketWrapper& sock)
{
	// Get the apropriate buffer
	Buffer* buffer = m_outBuffer.GetPacketBuffer(m_remote.m_nextSend);
	switch (m_nextType)
	{
	case GameData:
		// If we are using acks, we need to increment the index so we don't overwrite the buffer
		m_remote.m_nextSend++;
		break;
	case GameRequest:
	case ServerRequest:
	case Ping:
	default:
		break;
	}
	// Send the packet
	sock.Send(m_remote.m_address, buffer->GetBuffer(), (int) buffer->Size());
	return true;
}

void Connection::AckAndProcess(SocketWrapper& sock, Buffer& landing, void(*callback)(Buffer& data))
{
	Ack(sock, landing);

	AckHeader* inAck = m_inBuffer.GetAckHeader();

	// Process all ready packets that haven't been processed
	while (inAck->CheckAck(m_remote.m_nextEvaluate))
	{
		callback(*m_inBuffer.GetPacketBuffer(m_remote.m_nextEvaluate));
		m_remote.m_nextEvaluate++;
	}
}

void Connection::AckAndProcess(SocketWrapper& sock, Buffer& landing, void(*callback)(Buffer &data, int userId))
{
	Ack(sock, landing);

	AckHeader* inAck = m_inBuffer.GetAckHeader();

	// Process all ready packets that haven't been processed
	while (inAck->CheckAck(m_remote.m_nextEvaluate))
	{
		callback(*m_inBuffer.GetPacketBuffer(m_remote.m_nextEvaluate), m_remote.m_activeBuffer);
		m_remote.m_nextEvaluate++;
	}
}

void Connection::Ack(SocketWrapper& sock, Buffer& landing)
{
	// Need to figure out where to put the data
	AckHeader ack;
	Serializer::Deserialize<AckHeader>(landing, &ack);

	PacketData recieved;
	recieved.m_acked = true;

	// Ack the incoming packet
	m_inBuffer.SetPacketData(ack.m_packetId, recieved);

	Buffer* bData = m_inBuffer.GetPacketBuffer(ack.m_packetId);
	bData->MoveBuffer(landing);

	// Compare the incoming Ack with ack information we already have
	AckHeader* outAck = m_outBuffer.GetAckHeader();
	outAck->Ack(ack);
	

	// Check the acks we recieved to see if a package was lost
	unsigned int acksToCheck = outAck->m_lastIdRecieved - (m_remote.m_unconfirmed < m_remote.m_nextSend ? m_remote.m_unconfirmed : m_remote.m_nextSend);
	bool allAcked = true;
	for (unsigned int i = 0; i <= acksToCheck; i++)
	{
		if (!outAck->CheckAck(m_remote.m_unconfirmed + i))
		{
			if (allAcked) {
				m_remote.m_unconfirmed += i;
				acksToCheck -= i;
				i = 0;
			}
			allAcked = false;
			// Resend the lost packet
			Resend(sock, m_remote.m_unconfirmed + i);
		}
	}
	if (allAcked)
	{
		m_remote.m_unconfirmed += acksToCheck;
	}
}

bool Connection::Resend(SocketWrapper& sock, unsigned __int32 packetId)
{
	// No, we aren't going to resend a packet we haven't sent yet
	if (packetId >= m_remote.m_nextSend)
	{
		return false;
	}

	Buffer* buffer = m_outBuffer.GetPacketBuffer(packetId);
	sock.Send(m_remote.m_address, buffer->GetBuffer(), (int)buffer->Size());
	return true;
}
