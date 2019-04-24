#include "DataNetworkStructs.h"
#include "Serializer.h"

bool DeathInfo::Serialize(Buffer& buffer) const
{
	return m_killedBy.Serialize(buffer),
		Serializer::SerializeInteger<0, 100>(buffer, m_entityId),
		Serializer::SerializeInteger<0, 100>(buffer, m_image),
		Serializer::SerializeFloatFP(buffer, m_deathTime);
}

bool DeathInfo::Deserialize(Buffer& buffer)
{
	return m_killedBy.Deserialize(buffer),
		Serializer::DeserializeInteger<0, 100>(buffer, m_entityId),
		Serializer::DeserializeInteger<0, 100>(buffer, m_image),
		Serializer::DeserializeFloatFP(buffer, m_deathTime);
}

bool HostDataHeader::Serialize(Buffer& buffer) const
{
	return Serializer::Serialize<Phantom, 15>(buffer, m_phantoms, m_phantomCount),
		Serializer::Serialize<DeathInfo, 15>(buffer, m_deaths, m_deathCount),
		Serializer::Serialize<Phenomina, 15>(buffer, m_phenomina, m_phenominaCount);
}

bool HostDataHeader::Deserialize(Buffer& buffer)
{
	return Serializer::Deserialize<Phantom, 15>(buffer, m_phantoms, m_phantomCount),
		Serializer::Deserialize<DeathInfo, 15>(buffer, m_deaths, m_deathCount),
		Serializer::Deserialize<Phenomina, 15>(buffer, m_phenomina, m_phenominaCount);
}
