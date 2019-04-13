#pragma once

class PartitioningKey
{
public:
	//The depth that address are
	static const unsigned int DEPTH = 6;

private:
	//Masks that need to be used a lot
	const unsigned _int8 masks[2] = { 15, 15 << 4 };
	//bitset
	unsigned _int8 _address[(DEPTH + 1) / 2];

public:
	PartitioningKey();
	~PartitioningKey();

	//Copy assignment operator
	PartitioningKey& PartitioningKey::operator=(const PartitioningKey& other);

	//Sets a coordinate by index
	bool Set(int index, unsigned _int8 state);
	//Fetches a coordinate by index
	unsigned _int8 Get(int index);

	//Compares two Addresses to detect if they share space
	bool Compare(PartitioningKey &other);

};

