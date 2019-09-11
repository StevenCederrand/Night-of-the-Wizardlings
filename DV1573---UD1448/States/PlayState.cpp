#include <Pch/Pch.h>
#include "PlayState.h"

PlayState::PlayState()
{
	logTrace("Playstate created");

	printf("s or c?: ");
	char s; std::cin >> s;

	if (s == 's')
	{
		m_server.startup("My server");
		m_isServer = true;

		m_client.startup();
		m_client.connectToMyServer();
		m_isClient = true;
	}
	else if (s == 'c')
	{
		m_client.startup();
		m_isClient = true;
		m_client.refreshServerList();

		auto serverlist = m_client.getServerList();

		std::printf("Server list:\n");
		for (size_t i = 0; i < serverlist.size(); i++)
		{
			std::printf("Server %i: %s\n", i, serverlist[i].serverName);
		}

		std::printf("\nChoose server number: ");

		int i;
		std::cin >> i;

		m_client.connectToAnotherServer(serverlist[i]);
	}



}

PlayState::~PlayState()
{
	logTrace("Playstate destroyed");
}

void PlayState::update(float dt)
{
	if (m_isServer)
	{
		m_server.process();
	}
	if (m_isClient)
	{
		m_client.process();
	}



}

void PlayState::render()
{
}
