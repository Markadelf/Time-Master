#pragma once
#include "PacketBuffer.h"
#include "MessageHeader.h"
#include "SocketWrapper.h"

enum ConnectionErrorCodes {
	LostConnection
};

// Holds info about a connection
struct ConnectionInfo {
	Address m_address;						// IP Address
	int m_activeBuffer = -1;				// Used by server as a player ID
	unsigned __int32 m_nextEvaluate = 0;	// Next 
	unsigned __int32 m_unconfirmed = 0;
	unsigned __int32 m_nextSend = 0;
};

// Handles most of the acknowledgement logic and preps buffers for writing.
// Designed to avoid duplicate logic between client and server
class Connection
{
	// Store the last 32 outbound packets incase we need to resend
	PacketBuffer m_outBuffer;
	// Buffers for inbound messages that allow us to store up to 32 packets while waiting for a lost packet
	PacketBuffer m_inBuffer;

	// The address and info regarding the specifics of this connection
	ConnectionInfo m_remote;

	// Keeps track of the last message type we prepped
	MessageType m_nextType = MessageType::Ping;

public:
	Connection();
	~Connection();

	// Reinitializes the connection with info pointing to the remote location
	// Called once on client, may be called more than once by the server
	void Init(ConnectionInfo remote);

	// Prepares the next buffer and returns a pointer to it
	// Calling this more than once will reset the buffer
	// NOT THREAD SAFE FOR THE SAME CONNECTION
	Buffer* GetNextBuffer(MessageType messageType);

	// Sends the current buffer
	bool Send(SocketWrapper& sock);

	// Reads in the ack, checks the ack, sends data that is ready via callback
	void AckAndProcess(SocketWrapper& sock, Buffer& landing, void(*callback)(Buffer& data));
	void AckAndProcess(SocketWrapper& sock, Buffer& landing, void(*callback)(Buffer& data, int userId));

	void ResetAcks();

private:
	// Reads in the ack, checks the ack, makes any neccesary `Resend` calls
	void Ack(SocketWrapper& sock, Buffer& landing);

	// Resends a given buffer
	bool Resend(SocketWrapper& sock, unsigned __int32 packetId);
};

