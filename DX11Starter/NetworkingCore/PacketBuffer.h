#pragma once
#include <Buffer.h>
#include "AckHeader.h"

// Stores data regarding a packet
struct PacketData
{
	bool m_acked;
	PacketData() {
		m_acked = false;
	}
};

// Uses a ring buffer to store packets
class PacketBuffer
{
	// Data structure for acks only supports 32 packets
	static const int BufferSize = 32;

	// Highest sequence recieved/sent
	unsigned __int32 m_highestSeq;

	// Data buffers
	unsigned __int32 m_sequence_buffer[BufferSize];
	PacketData m_packetData[BufferSize];
	Buffer m_packets[BufferSize];

	// Ack info
	AckHeader m_ack;
public:
	PacketBuffer();
	~PacketBuffer();

	// Accessor
	PacketData* GetPacketData(unsigned __int16 sequence);
	Buffer* GetPacketBuffer(unsigned __int16 sequence);
	AckHeader* GetAckHeader();

	unsigned __int32 GetHighestSeq();

	// Modifier
	void SetPacketData(unsigned __int16 sequence, PacketData& data);

	// Clears the structure to ready it for new usage
	void Reset();
};

