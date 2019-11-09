#ifndef _DEFLECT_H
#define _DEFLECT_H
#include <Pch/Pch.h>
#include <GameObject/GameObject.h>

struct DeflectRenderInformation
{
	Mesh* mesh;
	Material* material;
};

class Deflect : public GameObject
{
public:
	Deflect(DeflectRenderInformation& renderInformation, const glm::vec3& position);
	virtual ~Deflect();
	virtual void update(float dt) = 0;

	const DeflectRenderInformation& getRenderInformation() const;

private:
	DeflectRenderInformation* m_deflectRenderInformation;
};


#endif // !_DEFLECT_H
