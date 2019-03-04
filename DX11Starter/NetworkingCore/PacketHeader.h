#pragma once
class PacketHeader
{
	unsigned __int16 packetId;
	unsigned __int16 lastIdRecieved;
	unsigned __int32 ackBits;
public:
	PacketHeader();
	~PacketHeader();


	void Acknowledge(unsigned __int16 lastIdRecieved);
	bool CheckAck(int offset);
};

