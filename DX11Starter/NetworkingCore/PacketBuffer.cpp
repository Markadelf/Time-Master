#include "stdafx.h"
#include "PacketBuffer.h"


PacketBuffer::PacketBuffer()
{
	for (size_t i = 0; i < BufferSize; i++)
	{
		packets[i] = Buffer(MaxPacketSize);
	}
}


PacketBuffer::~PacketBuffer()
{

}
