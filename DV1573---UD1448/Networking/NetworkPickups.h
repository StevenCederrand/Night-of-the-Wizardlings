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
		NetGlobals::THREAD_FLAG flag = NetGlobals::THREAD_FLAG::NONE;
	};

public:
	NetworkPickups();
	~NetworkPickups();

	void cleanUp();
	void update(const float& dt);

private:
	void setupHealthPickupRenderInformation(BGLoader* loader);
	void setupDamagePickupRenderInformation(BGLoader* loader);
	void cleanupRenderInformation(PickupRenderInformation* renderInformation);

private:
	friend class Client;
	std::mutex m_mutex;
	PickupRenderInformation m_healthRenderInformation;
	PickupRenderInformation m_damagaRenderInformation;
	std::vector<PickupProp> m_pickupProps;
};

#endif
