#include "PacketBuffer.h"
#include "Serializer.h"

PacketBuffer::PacketBuffer()
{
	for (size_t i = 0; i < BufferSize; i++)
	{
		m_packets[i] = Buffer(MaxPacketSize);
	}
}


PacketBuffer::~PacketBuffer()
{

}

PacketData* PacketBuffer::GetPacketData(unsigned __int16 sequence)
{
	const int index = sequence % BufferSize;
	if (m_sequence_buffer[index] == sequence)
		return &m_packetData[index];
	else
		return nullptr;
}

Buffer* PacketBuffer::GetPacketBuffer(unsigned __int16 sequence)
{
	const int index = sequence % BufferSize;
	if (m_sequence_buffer[index] == sequence)
		return &m_packets[index];
	else
		return nullptr;
}

AckHeader PacketBuffer::GetAckHeader()
{
	return m_ack;
}

unsigned __int32 PacketBuffer::GetHighestSeq()
{
	return m_highestSeq;
}

void PacketBuffer::SetPacketData(unsigned __int16 sequence, PacketData& data)
{
	if (data.m_acked)
	{
		m_ack.Ack(sequence);
	}
	m_highestSeq = m_highestSeq > sequence ? m_highestSeq : sequence;
	const int index = sequence % BufferSize;
	m_sequence_buffer[index] = sequence;
	m_packetData[index] = data;
}

