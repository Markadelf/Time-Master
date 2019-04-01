#include "AckHeader.h"
#include "Serializer.h"

AckHeader::AckHeader()
{
	m_packetId = 0;
	m_lastIdRecieved = 0;
	m_ackBits = 0;
}


AckHeader::~AckHeader()
{
}

void AckHeader::Ack(unsigned __int16 id)
{
	if (id > m_lastIdRecieved)
	{
		m_ackBits = (m_ackBits << (id - m_lastIdRecieved)) | 1;
		m_lastIdRecieved = id;
	}
	else
	{
		m_ackBits = m_ackBits | (1 << (m_lastIdRecieved - id));
	}
}

void AckHeader::Ack(const AckHeader& inAck)
{
	// Add the other ack to this ack
	if(inAck.m_lastIdRecieved > m_lastIdRecieved)
	{
		m_ackBits = (m_ackBits << (inAck.m_lastIdRecieved - m_lastIdRecieved)) | inAck.m_ackBits;
		m_lastIdRecieved = inAck.m_lastIdRecieved;
	}
	else
	{
		m_ackBits = m_ackBits | (inAck.m_lastIdRecieved << (m_lastIdRecieved - inAck.m_lastIdRecieved));
	}
}


bool AckHeader::CheckAck(unsigned __int16 id)
{
	// If an ack is out of range, throw it out
	if (id > m_lastIdRecieved || id <= m_lastIdRecieved - 32)
	{
		return false;
	}
	else
	{
		return (m_ackBits >> (m_lastIdRecieved - id)) & 1;
	}
}

bool AckHeader::CheckAckLost(unsigned __int16 id)
{
	// True if an ack is lost forever
	return m_lastIdRecieved - id > 31;
}

bool AckHeader::Serialize(Buffer& buffer) const
{
	return
		Serializer::SerializeUINT16(buffer, m_packetId) &&
		Serializer::SerializeUINT16(buffer, m_lastIdRecieved) &&
		Serializer::SerializeUINT32(buffer, m_ackBits);
}

bool AckHeader::Deserialize(Buffer& buffer)
{
	return
		Serializer::DeserializeUINT16(buffer, m_packetId) &&
		Serializer::DeserializeUINT16(buffer, m_lastIdRecieved) &&
		Serializer::DeserializeUINT32(buffer, m_ackBits);
}


