#include <Pch/Pch.h>
#include "PlayState.h"
// TODO move to mesh
#include <Loader/BGLoader.h>


PlayState::PlayState()
{
	m_shaderMap = m_shaderMap->getInstance();
	m_shaderMap->createShader("Basic_Forward", "VertexShader.vs", "FragShader.fs");


	// TODO move to mesh and file filepath
	m_object = GameObject("Character");
	m_object.loadMesh("TestScene.mesh");


	m_camera = new Camera();
	m_player = new Player("test", glm::vec3(0, 0, 3), m_camera);
	/*
	BGLoader tempLoader;
	tempLoader.LoadMesh(MESHPATH + "WalkingTest.mesh");

	m_mesh.setUpMesh(tempLoader.GetVertices(0),
		tempLoader.GetVertexCount(0),
		tempLoader.GetFaces(0),
		tempLoader.GetFaceCount(0));
	m_mesh.setUpBuffers();

	Material tempMaterial;
	tempMaterial = tempLoader.GetMaterial(0);	
	MaterialMap::getInstance()->createMaterial((std::string)tempLoader.GetMaterial(0).name, tempMaterial);
	tempLoader.Unload();*/


	m_renderer = m_renderer->getInstance();

	//printf("Start server? (s) or connect as a client? (c): ");
	//char s; std::cin >> s;

	//if (s == 's')
	//{	
	//	std::string name;
	//	std::cout << "\nEnter server name: ";
	//	std::cin >> name;
	//	m_server.startup(name);
	//	m_isServer = true;

	//	m_client.startup();
	//	m_client.connectToMyServer();
	//	m_isClient = true;
	//}
	//else if (s == 'c')
	//{
	//	m_client.startup();
	//	m_isClient = true;
	//	m_client.refreshServerList();

	//	auto serverlist = m_client.getServerList();

	//	std::printf("Server list:\n");
	//	for (size_t i = 0; i < serverlist.size(); i++)
	//	{
	//		std::printf("Server %i: %s\n", i, serverlist[i].serverName);
	//	}

	//	std::printf("\nChoose server number: ");

	//	int i;
	//	std::cin >> i;

	//	m_client.connectToAnotherServer(serverlist[i]);
	//}

}

PlayState::~PlayState()
{
	MaterialMap::getInstance()->destroy();
}

void PlayState::update(float dt)
{
	m_renderer->update(dt);
	m_player->update(dt);
}

void PlayState::render()
{
	m_object.bindMaterialToShader("Basic_Forward");
	Renderer::getInstance()->render(m_object.getMesh()->getBuffers(), m_object.getMesh()->getPos());



}
