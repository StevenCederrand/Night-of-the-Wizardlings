#ifndef _PICKUP_HANDLER_H
#define _PICKUP_HANDLER_H
#include <Pch/Pch.h>
#include <Pickups/HealthPickup.h>

class Client;

class NetworkPickups {
public:
	struct PickupProp {
		Pickup* pickup;
		PickupPacket packet;
		NetGlobals::THREAD_FLAG flag = NetGlobals::THREAD_FLAG::None;
	};

public:
	NetworkPickups();
	~NetworkPickups();

	void cleanUp();
	void update(const float& dt);

private:
	void setupHealthPickupRenderInformation(BGLoader* loader);
	void setupManaPickupRenderInformation(BGLoader* loader);
	void cleanupRenderInformation(PickupRenderInformation* renderInformation);

private:
	friend class Client;

	//struct MeshBox //Handles seperate transforms for same mesh
	//{
	//	std::string name; //This is kinda useless 
	//	Transform transform;
	//	Material* material;
	//	Mesh* mesh;
	//	glm::vec3 btoffset = glm::vec3(0.0f);
	//};

	PickupRenderInformation m_healthRenderInformation;
	PickupRenderInformation m_ManaRenderInformation;
	std::vector<PickupProp> m_pickupProps;
};

#endif
