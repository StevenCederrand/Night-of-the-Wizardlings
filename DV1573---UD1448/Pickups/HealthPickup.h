#ifndef _HEALTH_PICKUP_H
#define _HEALTH_PICKUP_H
#include <Pch/Pch.h>
#include "Pickup.h"

class HealthPickup : public Pickup {

public:
	HealthPickup(PickupRenderInformation& renderInformation, PickupType& pickupType, const glm::vec3& position, const uint64_t uniqueID);
	virtual ~HealthPickup();
	virtual void update(float dt) override;

private:
	
};

#endif
