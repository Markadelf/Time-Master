#pragma once
#include <Buffer.h>
#include "AckHeader.h"

struct PacketData
{
	bool m_acked;
	PacketData() {
		m_acked = false;
	}
};

class PacketBuffer
{
	static const int BufferSize = 1024;
	static const int MaxPacketSize = 1200;

	// Highest sequence recieved/sent
	unsigned __int32 m_highestSeq;
	unsigned __int32 m_sequence_buffer[BufferSize];
	PacketData m_packetData[BufferSize];
	Buffer m_packets[BufferSize];

	AckHeader m_ack;
public:
	PacketBuffer();
	~PacketBuffer();

	// Accessor
	PacketData* GetPacketData(unsigned __int16 sequence);
	Buffer* GetPacketBuffer(unsigned __int16 sequence);
	AckHeader GetAckHeader();

	unsigned __int32 GetHighestSeq();

	// Modifier
	void SetPacketData(unsigned __int16 sequence, PacketData& data);


};

