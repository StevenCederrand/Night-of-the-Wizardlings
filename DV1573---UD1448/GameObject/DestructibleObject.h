#ifndef _DESTRUCTIBLEOBJECT_h
#define _DESTRUCTIBLEOBJECT_h
#include <Pch/Pch.h>
#include <GameObject/GameObject.h>
class DstrGenerator;

class DestructibleObject : public GameObject
{
public:
	DestructibleObject(DstrGenerator* dstr);
	DestructibleObject(std::string name, DstrGenerator* dstr);
	~DestructibleObject();

	void update(float dt);

	void loadDestructible(std::string fileName, float size);
	void loadBasic(std::string name);
	void loadDefined(std::string name, std::vector<glm::vec2> polygon);
	void meshFromPolygon(std::string name);
	void set_destroyed(bool state) { m_destroyed = state; }

	const std::vector<glm::vec2>& getPolygon() const { return m_polygonFace; }
	const float& getScale() const { return m_scale; }
	const bool& is_destroyed() const { return m_destroyed; }
	DstrGenerator* getDstr() { return dstrRef; }

private:
	std::vector<glm::vec2> m_polygonFace;

	float m_scale = 0.0f;
	bool m_destroyed = false;

	DstrGenerator* dstrRef;
};

#endif

