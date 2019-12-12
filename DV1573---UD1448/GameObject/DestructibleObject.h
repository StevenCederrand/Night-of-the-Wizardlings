#ifndef _DESTRUCTIBLEOBJECT_h
#define _DESTRUCTIBLEOBJECT_h
#include <Pch/Pch.h>
#include <GameObject/GameObject.h>
class DstrGenerator;

class DestructibleObject : public GameObject
{
public:
	DestructibleObject(DstrGenerator* dstr, int index, float fallGravity, float event1Time);
	~DestructibleObject();

	void update(float dt);

	void loadDestructible(std::string fileName, float size);
	void loadDestructible(std::vector<Vertex> vertices_in, std::string name_in,
		Material newMaterial_in, std::string albedo_in, Transform transform, float size);
	void loadBasic(std::string name);
	void loadDefined(std::string name, std::vector<glm::vec2> polygon);
	void findVertices(const std::vector<Vertex>& vertices);
	void meshFromPolygon(std::string name);

	void set_destroyed(bool state) { m_destroyed = state; }
	void setLifetime(float time) { m_lifetime = time; }
	void setEvent1Time(float time) { m_ev1Time = time; }
	void setEvent2Time(float time) { m_ev2Time = time; }
	void setEvent3Time(float time) { m_ev3Time = time; }

	const std::vector<glm::vec2>& getPolygon() const { return m_polygonFace; }
	const std::vector<glm::vec2>& getUv() const { return m_polygonUV; }
	const float& getScale() const { return m_scale; }
	const float& getLifetime() const { return m_lifetime; }
	const int& getIndex() const { return m_index; }
	const bool& is_destroyed() const { return m_destroyed; }
	DstrGenerator* getDstr() { return dstrRef; }

private:
	std::vector<glm::vec2> m_polygonFace;
	std::vector<glm::vec2> m_polygonUV;

	float m_ev1Time;
	float m_ev2Time;
	float m_ev3Time;
	btVector3 m_fallGravity;

	float m_lifetime = 0.0f;
	float m_scale = 0.0f;
	bool m_destroyed = false;
	int m_dstrState = 0;
	int m_index = -1;

	DstrGenerator* dstrRef;
};

#endif

