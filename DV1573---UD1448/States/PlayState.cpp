#include <Pch/Pch.h>
#include "PlayState.h"

PlayState::PlayState()
{
	logTrace("Playstate created");

	printf("Start server? (s) or connect as a client? (c): ");
	char s; std::cin >> s;

	if (s == 's')
	{	
		std::string name;
		std::cout << "\nEnter server name: ";
		std::cin >> name;
		m_server.startup(name);
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
	


}

void PlayState::render()
{
}
