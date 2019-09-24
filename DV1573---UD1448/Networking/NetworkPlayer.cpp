#include <Pch/Pch.h>
#include "NetworkPlayer.h"

NetworkPlayer::NetworkPlayer()
{
	m_data.health = 100.0f;
	m_data.position = glm::vec3(0.0f, 0.0f, 0.0f);
	m_data.rotation = glm::vec3(0.0f);
	m_hasGameObject = false;
}

void NetworkPlayer::updateGameObject()
{
	if (m_gameObject != nullptr) {
		m_gameObject->translate(m_data.position);
	}
}

NetworkPlayer::~NetworkPlayer()
{
	if (m_hasGameObject) {
		logTrace("Deleted a network player");
		delete m_gameObject;
	}
}

const PlayerData& NetworkPlayer::getData() const
{
	return m_data;
}


void NetworkPlayer::initialize(const std::string& mesh)
{
	m_gameObject = new WorldObject();
	m_gameObject->loadMesh(mesh);
	m_hasGameObject = true;
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

GameObject* NetworkPlayer::getGameObjectPtr() const
{
	return m_gameObject;
}
