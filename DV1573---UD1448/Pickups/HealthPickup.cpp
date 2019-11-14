#include "Pch/Pch.h"
#include "HealthPickup.h"

HealthPickup::HealthPickup(PickupRenderInformation& renderInformation, PickupType& pickupType, const glm::vec3& position, const uint64_t uniqueID) :
	Pickup(renderInformation, pickupType, position, uniqueID)
{
}

HealthPickup::~HealthPickup()
{

}

void HealthPickup::update(float dt)
{
	/* Play some nice particles here? */
}

