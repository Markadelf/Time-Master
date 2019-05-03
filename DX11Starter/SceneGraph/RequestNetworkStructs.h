#pragma once
#include "Buffer.h"

enum ClientRequestType {
	Join,					// Requests to join queue
	Quit,					// Requests to be forgotten
	Confirm,				// Confirms that it recieved a request
};

struct ClientRequest
{
	ClientRequestType m_request;
	// In real time. Used for synchronization or prediction based on type of request
	float m_timeArgument;

	bool Serialize(Buffer& buffer) const;
	bool Deserialize(Buffer& buffer);
};

enum HostRequestType {
	Prepare,				// Requests the game prepares to start a session
	DeclareVictor,			// Declares a victor
	Kick,					// Informs the player they have been kicked
};

struct HostRequest
{
	HostRequestType m_request;
	// In real time. Used for synchronization or prediction based on type of request
	float m_timeArgument;
	int m_arg;

	bool Serialize(Buffer& buffer) const;
	bool Deserialize(Buffer& buffer);
};

//struct GamePreparationRequest {
//	int m_scene;
//	int m_playerEntityId;
//
//	bool Serialize(Buffer& buffer) const;
//	bool Deserialize(Buffer& buffer);
//};