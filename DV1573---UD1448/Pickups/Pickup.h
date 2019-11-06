#ifndef _PICKUP_H
#define _PICKUP_H
#include <Pch/Pch.h>
#include <GameObject/GameObject.h>

struct PickupRenderInformation {
	Mesh* mesh;
	Material* material;
};

class Pickup : public GameObject{

public:
	Pickup(PickupRenderInformation& renderInformation, PickupType& pickupType, const glm::vec3& position, const uint64_t uniqueID);
	virtual ~Pickup();
	virtual void update(float dt) = 0;
	
	const PickupType& getType() const;
	const PickupRenderInformation& getRenderInformation() const;
	const uint64_t& getUniqueID() const;

private:
	uint64_t m_uniqueID;
	PickupType* m_pickupType;
	PickupRenderInformation* m_pickupRenderInformation;

};


#endif
