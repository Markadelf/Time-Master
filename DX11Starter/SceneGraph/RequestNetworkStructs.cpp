#include "RequestNetworkStructs.h"
#include "Serializer.h"

bool ClientRequest::Serialize(Buffer& buffer) const
{
	return Serializer::SerializeInteger<ClientRequestType::Join, ClientRequestType::Quit>(buffer, m_request)
		&& Serializer::SerializeFloatFP(buffer, m_timeArgument);
}

bool ClientRequest::Deserialize(Buffer& buffer)
{
	int val;
	bool ret = Serializer::DeserializeInteger<ClientRequestType::Join, ClientRequestType::Quit>(buffer, val)
		&& Serializer::DeserializeFloatFP(buffer, m_timeArgument);
	m_request = (ClientRequestType)val;

	return ret;
}

bool HostRequest::Serialize(Buffer& buffer) const
{
	return Serializer::SerializeInteger<HostRequestType::Prepare, HostRequestType::Kick>(buffer, m_request)
		&& Serializer::SerializeFloatFP(buffer, m_timeArgument);
}

bool HostRequest::Deserialize(Buffer& buffer)
{
	int val;
	bool ret = Serializer::DeserializeInteger<HostRequestType::Prepare, HostRequestType::Kick>(buffer, val)
		&& Serializer::DeserializeFloatFP(buffer, m_timeArgument);
	m_request = (HostRequestType)val;

	return ret;
}

bool GamePreparationRequest::Serialize(Buffer& buffer) const
{
	return Serializer::SerializeInteger<0, 1>(buffer, m_scene) &&
		Serializer::SerializeInteger<0, 10>(buffer, m_playerEntityId);
}

bool GamePreparationRequest::Deserialize(Buffer& buffer)
{
	return Serializer::DeserializeInteger<0, 1>(buffer, m_scene) &&
		Serializer::DeserializeInteger<0, 10>(buffer, m_playerEntityId);
}
