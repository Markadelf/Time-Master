#include "stdafx.h"
#include "Buffer.h"


// Allocate buffer
Buffer::Buffer(int sizeInBytes) {
	bitIndex = 0;
	bitCapacity = sizeInBytes * 8;
	bytes = new unsigned __int8[sizeInBytes];
}
// Deallocate buffer
Buffer::~Buffer() {
	delete[] bytes;
}

const unsigned __int8* Buffer::GetBuffer() {
	return bytes;
}

size_t Buffer::Size() {
	return (bitIndex + 7) / 8;
}

size_t Buffer::RemainingBits() {
	return bitCapacity - Size();
}

bool Buffer::WriteBits(unsigned __int8 data, size_t bitCount) {
	int next = bitIndex + bitCount;
	if (next > bitCapacity)
	{
		return false;
	}
	unsigned int byteIndex = bitIndex >> 3;
	unsigned int bitOffSet = bitIndex & 0x7;

	unsigned __int8 currentMask = ~(0xFF << bitOffSet);
	bytes[byteIndex] = (bytes[byteIndex] & currentMask) | (data << bitOffSet);

	int extra = 8 - bitOffSet;

	if (extra < bitCount)
	{
		bytes[byteIndex + 1] = data >> extra;
	}

	bitIndex = next;
	return true;
}

bool Buffer::WriteBits(const void* data, size_t bitCount) {
	const unsigned __int8* srcByte = static_cast<const unsigned __int8*>(data);

	bool valid = true;
	while (bitCount > 8)
	{
		valid = valid && WriteBits(*srcByte, 8);
		++srcByte;
		bitCount -= 8;
	}
	if (bitCount > 0)
	{
		valid = valid && WriteBits(*srcByte, bitCount);
	}
	return valid;
}

bool Buffer::WriteBytes(const void* data, size_t byteCount) {
	return WriteBits(data, byteCount << 3);
}

bool Buffer::ReadBits(unsigned __int8& data, size_t bitCount) {
	int end = bitIndex + bitCount;
	if (end > bitCapacity)
	{
		return false;
	}

	int byteIndex = bitIndex >> 3;
	int bitOffSet = bitIndex & 0x7;

	unsigned __int8 firstMask = ~(0xFF << bitCount);
	//bytes[byteIndex] = (bytes[byteIndex] & currentMask) | (data << bitOffSet);

	data = (bytes[byteIndex] >> bitOffSet) & firstMask;
	if (bitCount + bitOffSet > 8) {
		int extra = (bitCount + bitOffSet) % 8;
		unsigned __int8 secondMask = ~(0xFF << extra);
		__int8 data2 = (bytes[byteIndex + 1] & secondMask) << (bitCount - extra);
		data = data | data2;
	}
	bitIndex = end;
	return true;
}

bool Buffer::ReadBits(void* data, size_t bitCount) {
	unsigned __int8* srcByte = static_cast<unsigned __int8*>(data);

	size_t i;
	bool valid = true;
	for (i = 0; i < bitCount / 8; i++)
	{
		valid = valid && ReadBits(*(srcByte + i), 8);
	}
	if (bitCount % 8)
	{
		valid = valid && ReadBits(*(srcByte + i), bitCount % 8);
	}
	return valid;
}

bool Buffer::ReadBytes(void* data, size_t byteCount) {
	return ReadBits(data, byteCount << 3);
}

void Buffer::ResetIndex()
{
	bitIndex = 0;
}
