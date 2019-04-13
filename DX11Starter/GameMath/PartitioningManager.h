#pragma once

#include "PartitioningKey.h"

// Uses partitioning 
class PartitioningManager
{
	float m_center[3];
	float m_halfDistance[3];

public:
	//The bool that flips the recording of Display logic
	static bool display;
	PartitioningManager();
	~PartitioningManager();

	//Sets up the center and bounds
	void Initialize(float* min, float* max);
	//Gets an address based on positional information
	PartitioningKey GetAddress(float* min, float* max);

private:
	//Helper function that identifies the octant.
	unsigned _int8 Oct(float* center, float* location);

};