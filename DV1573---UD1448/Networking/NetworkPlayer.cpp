#include <Pch/Pch.h>
#include "NetworkPlayer.h"

NetworkPlayer::NetworkPlayer()
{
	m_data.health = 100.0f;
	m_data.position = glm::vec3(0.0f);
	m_data.rotation = glm::vec3(0.0f);
}

NetworkPlayer::~NetworkPlayer()
{
}

const NetworkPlayer::Data& NetworkPlayer::getData() const
{
	return m_data;
}

void NetworkPlayer::Serialize(bool writeToStream, RakNet::BitStream& stream)
{
	stream.Serialize(writeToStream, m_data.guid);
	stream.Serialize(writeToStream, m_data.health);
	stream.Serialize(writeToStream, m_data.position);
	stream.Serialize(writeToStream, m_data.rotation);
}

std::string NetworkPlayer::toString() const
{
	std::string str;

	str = "Guid: " + std::string(m_data.guid.ToString()) + "\n";
	str += "Health: " + std::to_string(m_data.health) + "\n";
	str += "Position: (" + std::to_string(m_data.position.x) + ", " + std::to_string(m_data.position.y) + ", " + std::to_string(m_data.position.z) + ")" + "\n";
	str += "Rotation: (" + std::to_string(m_data.rotation.x) + ", " + std::to_string(m_data.rotation.y) + ", " + std::to_string(m_data.rotation.z) + ")";

	return str;
}
