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

		std::lock_guard<std::mutex> lockGuard(NetGlobals::UpdatePickupsMutex);

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
				Renderer::getInstance()->removeRenderObject(e.pickup, PICKUP);
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
	//BGLoader tempLoader;

	m_healthRenderInformation.mesh = new Mesh();
	m_healthRenderInformation.material = new Material();

	auto* mesh = m_healthRenderInformation.mesh;
	auto* mat = m_healthRenderInformation.material;

	loader->LoadMesh(MESHPATH + "DamagePickup.mesh");
	
	mesh->saveFilePath(loader->GetFileName(), 0);
	mesh->nameMesh(loader->GetMeshName());
	mesh->setUpMesh(loader->GetVertices(), loader->GetFaces());
	mesh->setUpBuffers();

	const Material& newMaterial = loader->GetMaterial();
	mat->ambient = newMaterial.ambient;
	mat->diffuse = newMaterial.diffuse;
	mat->name = newMaterial.name;
	mat->specular = newMaterial.specular;

	//MeshBox tempMeshBox;									// Meshbox holds the mesh identity and local transform to GameObject
	//std::string meshName = tempLoader.GetMeshName();
	//tempMeshBox.name = meshName;
	//tempMeshBox.transform = tempLoader.GetTransform();

	//if (!MaterialMap::getInstance()->existsWithName(mat->name)) 	// This creates the material if it does not exist (by name)
	//{
	//	if (tempLoader.GetAlbedo() != "-1")
	//	{
	//		std::string albedoFile = TEXTUREPATH + tempLoader.GetAlbedo();
	//		GLuint texture;
	//		glGenTextures(1, &texture);
	//		glBindTexture(GL_TEXTURE_2D, texture);
	//		// set the texture wrapping/filtering options (on the currently bound texture object)
	//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//		// load and generate the texture
	//		int width, height, nrChannels;
	//		unsigned char* data = stbi_load(albedoFile.c_str(), &width, &height, &nrChannels, STBI_rgb_alpha);
	//		if (data)
	//		{
	//			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	//			glGenerateMipmap(GL_TEXTURE_2D);

	//			mat->texture = true;
	//			mat->textureID.push_back(texture);
	//		}
	//		else
	//		{
	//			std::cout << "Failed to load texture" << std::endl;
	//		}
	//		stbi_image_free(data);
	//	}
	//	else
	//	{
	//		mat->texture = false;
	//	}
	//	//Get the material pointer so that we don't have to always search through the MatMap, when rendering
	//	tempMeshBox.material = MaterialMap::getInstance()->createMaterial(mat->name, *mat);
	//	logTrace("Material created: {0}", mat->name);
	//}
	//else {
	//	tempMeshBox.material = MaterialMap::getInstance()->getMaterial(mat->name);
	//}

	loader->Unload();

	mat->diffuse = glm::vec3(1.0f, 0.0f, 0.0f);
	mat->ambient = glm::vec3(1.f, 0.0f, 0.0f);

	mesh->setMaterial(mat->name);
}

void NetworkPickups::setupDamagePickupRenderInformation(BGLoader* loader)
{
	//BGLoader tempLoader;

	m_damagaRenderInformation.mesh = new Mesh();
	m_damagaRenderInformation.material = new Material();

	auto* mesh = m_damagaRenderInformation.mesh;
	auto* mat = m_damagaRenderInformation.material;

	loader->LoadMesh(MESHPATH + "DamagePickup.mesh");

	mesh->saveFilePath(loader->GetFileName(), 0);
	mesh->nameMesh(loader->GetMeshName());
	mesh->setUpMesh(loader->GetVertices(), loader->GetFaces());
	mesh->setUpBuffers();

	const Material& newMaterial = loader->GetMaterial();
	mat->ambient = newMaterial.ambient;
	mat->diffuse = newMaterial.diffuse;
	mat->name = newMaterial.name;
	mat->specular = newMaterial.specular;

	//MeshBox tempMeshBox;									// Meshbox holds the mesh identity and local transform to GameObject
	//std::string meshName = tempLoader.GetMeshName();
	//tempMeshBox.name = meshName;
	//tempMeshBox.transform = tempLoader.GetTransform();

	//if (!MaterialMap::getInstance()->existsWithName(mat->name)) 	// This creates the material if it does not exist (by name)
	//{
	//	if (tempLoader.GetAlbedo() != "-1")
	//	{
	//		std::string albedoFile = TEXTUREPATH + tempLoader.GetAlbedo();
	//		GLuint texture;
	//		glGenTextures(1, &texture);
	//		glBindTexture(GL_TEXTURE_2D, texture);
	//		// set the texture wrapping/filtering options (on the currently bound texture object)
	//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//		// load and generate the texture
	//		int width, height, nrChannels;
	//		unsigned char* data = stbi_load(albedoFile.c_str(), &width, &height, &nrChannels, STBI_rgb_alpha);
	//		if (data)
	//		{
	//			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	//			glGenerateMipmap(GL_TEXTURE_2D);

	//			mat->texture = true;
	//			mat->textureID.push_back(texture);
	//		}
	//		else
	//		{
	//			std::cout << "Failed to load texture" << std::endl;
	//		}
	//		stbi_image_free(data);
	//	}
	//	else
	//	{
	//		mat->texture = false;
	//	}
	//	//Get the material pointer so that we don't have to always search through the MatMap, when rendering
	//	tempMeshBox.material = MaterialMap::getInstance()->createMaterial(mat->name, *mat);
	//	logTrace("Material created: {0}", mat->name);
	//}
	//else {
	//	tempMeshBox.material = MaterialMap::getInstance()->getMaterial(mat->name);
	//}

	loader->Unload();

	mat->diffuse = glm::vec3(1.0f, 1.0f, 0.0f);
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
