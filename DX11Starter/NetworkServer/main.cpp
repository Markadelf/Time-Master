// NetworkingPlayground.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <Serializer.h>
#include <TimeInstableTransform.h>
#include "ServerManager.h"
#include "ServerSceneGraph.h"
#include "ClientHelper.h"
#include "GameHost.h"

void ClientCallback(Buffer& data, int clientId);
void ActiveCallback(Buffer& data, int activeId);
void ReadBuffer(Buffer& bitBuffer);
void ReadBuffer(Address ad, const void* data, const int size);

ServerManager* serverPointer;
//GameHost* gameHost;
ServerSceneGraph graph;

int main()
{
	int port = 30000;
	ServerManager server(port, 6, 2);
	serverPointer = &server;
	Address serverAddress(127, 0, 0, 1, port);
	server.SetClientCallBack(ClientCallback);
	server.SetActiveCallBack(ActiveCallback);
	
	// Add static objects to scene graph
	const int div = 20;
	StaticObject objs[div + 1];
	Vector2 right = Vector2(5, 0);
	HandleObject handle;
	handle.m_scale[2] = 2;
	handle.m_collider = graph.GetColliderHandle(Colliders2D::ColliderType::Rectangle, 1, 2);

	for (size_t i = 0; i < div; i++)
	{
		Transform t(right.Rotate(6.28f / div * i), -6.28f / div * i);
		objs[i] = StaticObject(t, handle);
	}
	handle.SetUniformScale(.01f);
	handle.m_collider = graph.GetColliderHandle(Colliders2D::ColliderType::Circle, .5f);
	//handle.m_scale[2] = 1;

	Transform t(Vector2(), 0);
	objs[div] = StaticObject(t, handle);

	graph.Init(&objs[0], div + 1);

	handle.m_collider = graph.GetColliderHandle(Colliders2D::ColliderType::Circle, .25f);
	handle.m_scale[0] = 1;
	handle.m_scale[1] = 1;
	handle.m_scale[2] = 1;
	// Add player
	int id = graph.AddEntity(2048, 100);
	graph.GetEntity(id)->Initialize(Transform(Vector2(0, 0), 0), 0, handle);

	// Add another player
	id = graph.AddEntity(2048, 100);
	graph.GetEntity(id)->Initialize(Transform(Vector2(0, 0).Rotate(3.14f / 3 * 2), 0), 0, handle);

	// Add another player
	id = graph.AddEntity(2048, 100);
	graph.GetEntity(id)->Initialize(Transform(Vector2(0, 0).Rotate(-3.14f / 3 * 2), 0), 0, handle);

	while (true)
	{
		server.Listen();
	}
}

void ClientCallback(Buffer& bitBuffer, int clientId) {
	//gameHost->HostRecieveClient(bitBuffer, clientId, serverPointer);
	int userId = 0;
	serverPointer->SetUser(clientId, userId);
}

void ActiveCallback(Buffer& bitBuffer, int playerId) {
	//gameHost->HostRecievePlayer(bitBuffer, playerId, serverPointer);
	graph.HostRecieve(bitBuffer, playerId, serverPointer);
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
