#ifndef _PICKUP_NOTIFICATION_STRUCTURE_H
#define _PICKUP_NOTIFICATION_STRUCTURE_H
#include <Pch/Pch.h>

struct PickupNotificationText {
	float alphaColor;
	std::vector<std::pair<std::string, glm::vec3>> textParts;
	unsigned int width = 0;
	glm::vec3 scale = glm::vec3(1.0f);
};


#endif
