#ifndef _DEFLECT_RENDER_H
#define _DEFLECT_RENDER_H
#include <Pch/Pch.h>
#include "Deflect.h"

class DeflectRender : public Deflect
{
public:
	DeflectRender(DeflectRenderInformation& renderInformation, const glm::vec3& position);
	virtual ~DeflectRender();
	virtual void update(float dt) override;
private:

};


#endif