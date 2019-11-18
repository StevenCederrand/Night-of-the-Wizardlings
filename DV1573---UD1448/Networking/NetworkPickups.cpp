#include "Pch/Pch.h"
#include "NetworkPickups.h"

#include <Renderer/Renderer.h>
#include <Networking/Client.h>

NetworkPickups::NetworkPickups()
{
	
	BGLoader* loader = new BGLoader();
	setupHealthPickupRenderInformation(loader);
	setupDamagePickupRenderInformation(loader);
	delete loader;

}

NetworkPickups::~NetworkPickups()
{
	
}

void NetworkPickups::cleanUp()
{

	cleanupRenderInformation(&m_healthRenderInformation);
	cleanupRenderInformation(&m_damagaRenderInformation);

	for (size_t i = 0; i < m_pickupProps.size(); i++) {
		if(m_pickupProps[i].pickup != nullptr)
			delete m_pickupProps[i].pickup;
	}
}

void NetworkPickups::update(const float& dt)
{
	auto* clientPtr = Client::getInstance();

	if (clientPtr->isConnectedToSever()) {

		std::lock_guard<std::mutex> lockGuard(NetGlobals::m_updatePickupsMutex);

		for (size_t i = 0; i < m_pickupProps.size(); i++) {

			auto& e = m_pickupProps[i];

			if (e.flag == NetGlobals::THREAD_FLAG::Add) {
				if (e.pickup == nullptr) {

					if (e.packet.type == PickupType::HealthPotion) {
						e.pickup = new HealthPickup(m_healthRenderInformation, e.packet.type, e.packet.position, e.packet.uniqueID);
					}
					else if (e.packet.type == PickupType::DamageBuff) {
						e.pickup = new HealthPickup(m_damagaRenderInformation, e.packet.type, e.packet.position, e.packet.uniqueID);
					}
					
					
					Renderer::getInstance()->submit(e.pickup, PICKUP); 
					e.flag = NetGlobals::THREAD_FLAG::None;

				}
			}
			else if (e.flag == NetGlobals::THREAD_FLAG::Remove)
			{
				Renderer::getInstance()->removeDynamic(e.pickup, PICKUP);
				delete e.pickup;
				m_pickupProps.erase(m_pickupProps.begin() + i);
				i--;
				continue;
				
			}

		}
	}
}

void NetworkPickups::setupHealthPickupRenderInformation(BGLoader* loader)
{
	m_healthRenderInformation.mesh = new Mesh();
	m_healthRenderInformation.material = new Material();

	auto* mesh = m_healthRenderInformation.mesh;
	auto* mat = m_healthRenderInformation.material;

	loader->LoadMesh(MESHPATH + "dragonfirepotion.mesh");
	
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
	mat->ambient = glm::vec3(1.0f, 0.0f, 0.0f);

	mesh->setMaterial(mat->name);
}

void NetworkPickups::setupDamagePickupRenderInformation(BGLoader* loader)
{
	m_damagaRenderInformation.mesh = new Mesh();
	m_damagaRenderInformation.material = new Material();

	auto* mesh = m_damagaRenderInformation.mesh;
	auto* mat = m_damagaRenderInformation.material;

	loader->LoadMesh(MESHPATH + "dragonfirepotion.mesh");

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
	mat->ambient = glm::vec3(1.f, 1.0f, 0.0f);

	mesh->setMaterial(mat->name);
}

void NetworkPickups::cleanupRenderInformation(PickupRenderInformation* renderInformation)
{
	if (renderInformation->mesh != nullptr) {
		renderInformation->mesh->Destroy();
		delete renderInformation->mesh;
	}

	if (renderInformation->material != nullptr) {
		delete renderInformation->material;
	}
}
