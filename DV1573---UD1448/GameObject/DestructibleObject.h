#ifndef _DESTRUCTIBLEOBJECT_h
#define _DESTRUCTIBLEOBJECT_h
#include <GameObject/GameObject.h>

class DestructibleObject : public GameObject
{
public:
	DestructibleObject();
	DestructibleObject(std::string name);
	~DestructibleObject();

	void update(float dt);

	void loadDestructible(std::string fileName);
	void loadBasic(std::string name);
	void loadDefined(std::string name, std::vector<glm::vec2> polygon);
	void set_destroyed(bool state) { m_destroyed = state; }

	const std::vector<glm::vec2>& getPolygon() const { return m_polygonFace; }
	const float& getScale() const { return m_scale; }
	const bool& is_destroyed() const { return m_destroyed; }

private:
	std::vector<glm::vec2> m_polygonFace;

	float m_scale = 0.0f;
	bool m_destroyed = false;

};

#endif

