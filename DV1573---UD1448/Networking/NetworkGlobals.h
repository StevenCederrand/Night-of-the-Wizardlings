#ifndef _NETWORKGLOBALS_H
#define _NETWORKGLOBALS_H

namespace NetGlobals {

	constexpr unsigned int MaximumConnections = 4;
	constexpr unsigned short MaximumIncomingConnections = 4;
	constexpr unsigned short ServerPort = 42405;
	constexpr int networkRefreshRate = 30; // How many ms of sleep after each iteration on server/client
}

#endif
