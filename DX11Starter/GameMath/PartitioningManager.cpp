#include "PartitioningManager.h"

PartitioningManager::PartitioningManager()
{
}


PartitioningManager::~PartitioningManager()
{
}

void PartitioningManager::Initialize(float* min, float* max)
{
	for (size_t i = 0; i < 3; i++)
	{
		m_center[i] = (min[i] + max[i]) / 2;
		m_halfDistance[i] = max[i] - m_center[i];
	}
}

//Get an address from the Octree
PartitioningKey PartitioningManager::GetAddress(float* min, float* max)
{
	PartitioningKey ret = PartitioningKey();

	float center[3];
	//= m_center[3];
	float bounds[3];
	// = _halfDistance;

	//For each level of recursion
	for (size_t i = 0; i < PartitioningKey::DEPTH; i++)
	{
		//Get the octant of the max and the octant of the min
		unsigned _int8 a = Oct(center, min);
		unsigned _int8 b = Oct(center, max);
		//If they are the same and neither is zero, 
		if (a == b && a != 0) {
			//We have another level of specificity, so assign it to the address and subdivide
			ret.Set(i, a);
			for (size_t i = 0; i < 3; i++)
			{
				bounds[i] /= 2;
				center[i] += bounds[i] * (min[i] > center[i] ? 1 : -1);
			}
		}
		else
		{
			return ret;
		}
	}
	return ret;
}

//Returns a 4 bit bitset that is as follows:
//index: meaning
//0: Is specific
//1: x is high
//2: y is high
//3: z is high
unsigned _int8 PartitioningManager::Oct(float* center, float* location)
{
	if (location[0] == center[0] || location[1] == center[1] || location[2] == center[2])
		return 0;
	_int8 ret = 1;
	if (location[0] > center[0])
		ret = ret | (1 << 1);
	if (location[1] > center[1])
		ret = ret | (1 << 2);
	if (location[2] > center[2])
		ret = ret | (1 << 3);
	return ret;
}
