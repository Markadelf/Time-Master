#pragma once
#include "Phantom.h"

// Struct detailing the circumstances of a player's death
struct DeathInfo {
	PhenominaHandle m_killedBy;
	int m_entityId;
	int m_image;
	TimeStamp m_deathTime;

	bool Serialize(Buffer& buffer) const;
	bool Deserialize(Buffer& buffer);
};

// A serializable data message that is sent by the server to clients
struct HostDataHeader
{
	Phantom m_phantoms[15];
	int m_phantomCount = 0;

	DeathInfo m_deaths[15];
	int m_deathCount = 0;

	Phenomina m_phenomina[15];
	int m_phenominaCount = 0;

	bool Serialize(Buffer& buffer) const;
	bool Deserialize(Buffer& buffer);
};

