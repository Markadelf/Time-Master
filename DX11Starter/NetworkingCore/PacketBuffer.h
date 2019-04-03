#pragma once
#include <Buffer.h>

class PacketBuffer
{
	static const int BufferSize = 1024;
	static const int MaxPacketSize = 1200;

	unsigned __int32 sequence_buffer[BufferSize];
	Buffer packets[BufferSize];

public:
	PacketBuffer();
	~PacketBuffer();
};

