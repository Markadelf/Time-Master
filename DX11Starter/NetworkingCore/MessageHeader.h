#pragma once
#include "AckHeader.h"
#include "Serializer.h"
#include "Address.h"

enum MessageType {
	Ping,			// Does Nothing
	GameData,		// Data acked and used for game loop
	GameRequest,	// Data unacked and used for making requests of the game server
	ServerRequest	// Data unacked and used for communication internal to the networking system
};

struct MessageHeader {
	MessageType m_type;

	bool Serialize(Buffer& buffer) const {
		return Serializer::SerializeInteger<(int)MessageType::Ping, (int)MessageType::ServerRequest>(buffer, m_type);
	}
	bool Deserialize(Buffer& buffer) {
		int val;
		bool ret = Serializer::DeserializeInteger<(int)MessageType::Ping, (int)MessageType::ServerRequest>(buffer, val);
		m_type = (MessageType)val;
		return ret;
	}
};