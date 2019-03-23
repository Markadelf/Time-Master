// NetworkingPlayground.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <SocketWrapper.h>
#include <Buffer.h>
#include <Serializer.h>
#include <TimeInstableTransform.h>

void PrintMessage(Address, const void*, const int);
void ReadBuffer(Buffer& bitBuffer);
int main()
{
	int port = 30000;
	SocketWrapper::StartUp();
	SocketWrapper sock(port);

	const char data[] = "foobar";

	Buffer bitBuffer(2512);

	TimeInstableTransform trans;

	int pack = 8;
	for (size_t i = 0; i < 10; i++)
	{
		trans = TimeInstableTransform(Transform(Vector2(i, i), i / 100.f), Transform(Vector2(i + 1, i + 1), i / 100.f + .1f), i, i + 1, i % 2 == 0);
		if (!Serializer::SerializeFloat<5, 15, 1, 10>(bitBuffer, 5 + i * .1f) ||
			!Serializer::SerializeString<20>(bitBuffer, data) ||
			!Serializer::SerializeUINT32(bitBuffer, i + 5) ||
			!Serializer::Serialize<TimeInstableTransform>(bitBuffer, &trans)) {
			std::cout << "shit" << std::endl;
		}
	}

	sock.Send(Address(127, 0, 0, 1, port), bitBuffer.GetBuffer(), bitBuffer.Size());

	sock.Recieve(PrintMessage);

	SocketWrapper::CleanUp();
}

void PrintMessage(Address ad, const void* data, const int size) {
	Buffer inBuffer(size);
	inBuffer.WriteBytes(data, size);
	inBuffer.ResetIndex();
	ReadBuffer(inBuffer);
}

void ReadBuffer(Buffer& bitBuffer) {
	for (size_t i = 0; i < 10; i++)
	{
		float val = 0;
		if (Serializer::DeserializeFloat<5, 15, 1, 10>(bitBuffer, val)) {
			std::cout << val << std::endl;
		}
		else {
			std::cout << "shit" << std::endl;
		}
		char output[21];

		if (Serializer::DeserializeString<20>(bitBuffer, output)) {
			std::cout << output << std::endl;
		}
		else {
			std::cout << "shit" << std::endl;
		}

		unsigned int iVal = 0;
		if (Serializer::DeserializeUINT32(bitBuffer, iVal)) {
			std::cout << iVal << std::endl;
		}
		else {
			std::cout << "shit" << std::endl;
		}

		TimeInstableTransform tValue;
		if (Serializer::Deserialize<TimeInstableTransform>(bitBuffer, &tValue)) {
			Transform a = tValue.GetTransform(tValue.GetStartTime());
			Transform b = tValue.GetTransform(tValue.GetEndTime());
			std::cout << a.GetPos().GetX() << " " << a.GetPos().GetY() << " " << a.GetRot() << std::endl;
			std::cout << b.GetPos().GetX() << " " << b.GetPos().GetY() << " " << b.GetRot() << std::endl;
			std::cout << i << " is " << (tValue.GetReversed() ? "even" : "odd") << std::endl;
		}
		else {
			std::cout << "shit" << std::endl;
		}
	}
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
