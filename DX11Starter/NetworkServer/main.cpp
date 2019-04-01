// NetworkingPlayground.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <Serializer.h>
#include <TimeInstableTransform.h>
#include "ServerManager.h"
#include "ServerSceneGraph.h"
#include "ClientHelper.h"

void ClientCallback(Buffer& data, int clientId);
void ActiveCallback(Buffer& data, int activeId);
void ReadBuffer(Buffer& bitBuffer);
void ReadBuffer(Address ad, const void* data, const int size);

ServerSceneGraph graph(100, 100, 10);

int main()
{
	int port = 30000;
	ServerManager server(port, 6, 2);
	Address serverAddress(127, 0, 0, 1, port);
	server.SetClientCallBack(ClientCallback);
	server.SetActiveCallBack(ActiveCallback);
	
	const char word[] = "Hello World";
	const char serverWord[] = "Hello Client";
	ClientHelper a(port + 1, serverAddress);
	a.SetActiveCallBack(ReadBuffer);
	a.SetClientCallBack(ReadBuffer);
	
	Buffer* data = a.GetNextBuffer(MessageType::Ping);
	Serializer::SerializeString<30>(*data, &word[0]);

	a.SendToServer();
	server.Listen();

	data = a.GetNextBuffer(MessageType::GameData);
	Serializer::SerializeString<30>(*data, &word[0]);
	a.SendToServer();
	server.Listen();

	server.SetUser(0, 0);

	while (true)
	{
		server.Listen();
		a.Listen();
		data = a.GetNextBuffer(MessageType::GameData);
		Serializer::SerializeString<30>(*data, &word[0]);
		a.SendToServer();
		data = server.GetNextBufferActiveUser(MessageType::GameData, 0);
		Serializer::SerializeString<30>(*data, &serverWord[0]);
		server.SendToActiveUser(0);
	}
}

void ClientCallback(Buffer& bitBuffer, int clientId) {
	char name[31];
	Serializer::DeserializeString<30>(bitBuffer, name);
	std::cout << "Client " << clientId << ": " << name << std::endl;
}

void ActiveCallback(Buffer& bitBuffer, int playerId) {
	char name[31];
	Serializer::DeserializeString<30>(bitBuffer, name);
	std::cout << "Player " << playerId << ": " << name << std::endl;
}

void ReadBuffer(Buffer& bitBuffer) {
	char output[21];

	if (Serializer::DeserializeString<30>(bitBuffer, output)) {
		std::cout << output << std::endl;
	}
}

void ReadBuffer(Address ad, const void* data, const int size)
{
	Buffer buff(size);
	buff.WriteBytes(data, size);
	buff.ResetIndex();
	ReadBuffer(buff);
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
