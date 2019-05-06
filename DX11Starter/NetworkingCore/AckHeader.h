#pragma once
#include "Buffer.h"
// structure used to send ackdata over the internet
struct AckHeader
{
	// Id of this packet
	unsigned __int16 m_packetId;

	// Highest id of packets recieved
	unsigned __int16 m_lastIdRecieved;
	// Ack data for the last 32 acks
	unsigned __int32 m_ackBits;

	AckHeader();
	~AckHeader();

	// Ack a packet -Used for inbound acks
	void Ack(unsigned __int16 id);
	// Ack an ack buffer -Used for recieving acks from network
	void Ack(const AckHeader& inAck);
	// Check the ack of a sequence. False if unacked or if fallen off
	bool CheckAck(unsigned __int16 id);
	// Check if an ack is fully lost
	bool CheckAckLost(unsigned __int16 id);

	bool Serialize(Buffer& buffer) const;
	bool Deserialize(Buffer& buffer);
};

