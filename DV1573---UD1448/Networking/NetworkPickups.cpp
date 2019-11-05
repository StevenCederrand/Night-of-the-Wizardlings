#include "Pch/Pch.h"
#include "NetworkPickups.h"

#include <Renderer/Renderer.h>
#include <Networking/Client.h>

NetworkPickups::NetworkPickups()
{
	
	BGLoader* loader = new BGLoader();
	setupHealthPickupRenderInformation(loader);
	delete loader;

}

NetworkPickups::~NetworkPickups()
{
	cleanupRenderInformation(&m_healthRenderInformation);
}

void NetworkPickups::update(const float& dt)
{
	auto* clientPtr = Client::getInstance();

	if (clientPtr->isConnectedToSever()) {

		std::lock_guard<std::mutex> lockGuard(m_mutex);

		for (size_t i = 0; i < m_pickupProps.size(); i++) {

			auto& e = m_pickupProps[i];

			if (e.flag == NetGlobals::THREAD_FLAG::ADD) {
				if (e.pickup == nullptr) {

					//e.gameobject = new WorldObject();
					if (e.packet.type == PickupType::HealthPotion) {
						e.pickup = new HealthPickup(m_healthRenderInformation, e.packet.type, e.packet.position, e.packet.uniqueID);
					}

					/*e.gameobject->setWorldPosition(e.spellData.Position);
					Renderer::getInstance()->submit(e.gameobject, SPELL); */
					e.flag = NetGlobals::THREAD_FLAG::NONE;

				}
			}
			else if (e.flag == NetGlobals::THREAD_FLAG::REMOVE)
			{
				/*Renderer::getInstance()->removeDynamic(e.gameobject, SPELL);
				delete e.gameobject;
				m_entities.erase(m_entities.begin() + i);
				i--;
				continue;*/
			}

			//GameObject* g = e.gameobject;

			//if (g != nullptr) {
			//	glm::vec3 pos = CustomLerp(g->getTransform().position, e.spellData.Position, m_lerpSpeed * dt);
			//	g->setWorldPosition(pos);
			//	//g->setTransform(pos, glm::quat(p->data.rotation));

			//}
		}
	}
}

void NetworkPickups::setupHealthPickupRenderInformation(BGLoader* loader)
{
	m_healthRenderInformation.mesh = new Mesh();
	m_healthRenderInformation.material = new Material();

	auto* mesh = m_healthRenderInformation.mesh;
	auto* mat = m_healthRenderInformation.material;

	loader->LoadMesh(MESHPATH + "TestSphere.mesh");
	mesh->saveFilePath(loader->GetFileName(), 0);
	mesh->nameMesh(loader->GetMeshName());
	mesh->setUpMesh(loader->GetVertices(), loader->GetFaces());
	mesh->setUpBuffers();

	const Material& newMaterial = loader->GetMaterial();
	mat->ambient = newMaterial.ambient;
	mat->diffuse = newMaterial.diffuse;
	mat->name = newMaterial.name;
	mat->specular = newMaterial.specular;
	loader->Unload();

	mat->diffuse = glm::vec3(1.0f, 0.0f, 0.0f);
	mat->ambient = glm::vec3(0.65f, 1.0f, 1.0f);
}

void NetworkPickups::cleanupRenderInformation(PickupRenderInformation* renderInformation)
{
	delete renderInformation->mesh;
	delete renderInformation->material;
}
