#include "stdafx.h"
#include "AckBuffer.h"


AckBuffer::AckBuffer()
{
}


AckBuffer::~AckBuffer()
{
}

PacketData* AckBuffer::GetPacketData(unsigned __int16 sequence)
{
	const int index = sequence % BufferSize;
	if (sequence_buffer[index] == sequence)
		return &packet_data[index];
	else
		return nullptr;
}

void AckBuffer::SetPacketData(unsigned __int16 sequence, PacketData& data) 
{
	const int index = sequence % BufferSize;
	sequence_buffer[index] = sequence;
	packet_data[index] = data;
}
