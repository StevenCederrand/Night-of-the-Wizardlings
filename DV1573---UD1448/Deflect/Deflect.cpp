#include <Pch/Pch.h>
#include "Deflect.h"

Deflect::Deflect(DeflectRenderInformation& renderInformation, const glm::vec3& position)
{
	m_deflectRenderInformation = &renderInformation;
	setWorldPosition(position);
}

Deflect::~Deflect()
{
}

const DeflectRenderInformation& Deflect::getRenderInformation() const
{
	return *m_deflectRenderInformation;
}
