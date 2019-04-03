#pragma once
struct AckHeader
{
	unsigned __int16 packetId;
	unsigned __int16 lastIdRecieved;
	unsigned __int32 ackBits;

	AckHeader();
	~AckHeader();
};

