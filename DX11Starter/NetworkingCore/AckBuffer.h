#pragma once

struct PacketData
{
	bool acked;
};

// Acknowledgement ring buffer
class AckBuffer
{
	static const int BufferSize = 1024;

	unsigned __int32 sequence_buffer[BufferSize];
	PacketData packet_data[BufferSize];


public:
	AckBuffer();
	~AckBuffer();

	PacketData* GetPacketData(unsigned __int16 sequence);
	void SetPacketData(unsigned __int16 sequence, PacketData& data);
};

