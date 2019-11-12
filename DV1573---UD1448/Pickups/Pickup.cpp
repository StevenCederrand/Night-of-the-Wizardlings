#include "Pch/Pch.h"
#include "Pickup.h"

Pickup::Pickup(PickupRenderInformation& renderInformation, PickupType& pickupType, const glm::vec3& position, const uint64_t uniqueID)
{
	m_pickupType = &pickupType;
	m_pickupRenderInformation = &renderInformation;
	m_uniqueID = uniqueID;
	setWorldPosition(position);
}



Pickup::~Pickup()
{
}

const PickupType& Pickup::getType() const
{
	return *m_pickupType;
}

const PickupRenderInformation& Pickup::getRenderInformation() const
{
	return *m_pickupRenderInformation;
}

const uint64_t& Pickup::getUniqueID() const
{
	return m_uniqueID;
}
