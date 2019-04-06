#pragma once

// Buffer for outputting messages
class Buffer
{
	// Keep track of data structure at the bit level
	unsigned int bitCapacity;
	unsigned int bitIndex;
	unsigned __int8* bytes;

public:
	Buffer();
	Buffer(int sizeInBytes);
	~Buffer();

	const unsigned __int8* GetBuffer();

	// Size of FILLED buffer in bytes
	size_t Size();
	// Number of bits left
	size_t RemainingBits();

	// Write bits out of a byte
	bool WriteBits(unsigned __int8 data, unsigned int bitCount);
	// Write bits out of an buffer
	bool WriteBits(const void* data, unsigned int bitCount);
	// Writes bytes out of a buffer
	bool WriteBytes(const void* data, unsigned int byteCount);

	// Read bits into a byte
	bool ReadBits(unsigned __int8& data, unsigned int bitCount);
	// Read bits into an array
	bool ReadBits(void* data, unsigned int bitCount);
	// Read bytes into a buffer
	bool ReadBytes(void* data, unsigned int byteCount);

	// Resets the index for READ or WRITE
	void ResetIndex();
};

