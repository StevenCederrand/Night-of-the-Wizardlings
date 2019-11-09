#include <Pch/Pch.h>
#include "DeflectRender.h"

DeflectRender::DeflectRender(DeflectRenderInformation& renderInformation, const glm::vec3& position) :
	Deflect(renderInformation, position)
{
}

DeflectRender::~DeflectRender()
{
}

void DeflectRender::update(float dt)
{
}
