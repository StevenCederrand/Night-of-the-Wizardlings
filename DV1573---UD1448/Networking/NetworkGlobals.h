#ifndef _NETWORKGLOBALS_H
#define _NETWORKGLOBALS_H

namespace NetGlobals {

	constexpr unsigned int MaximumConnections = 12;
	constexpr unsigned short MaximumIncomingConnections = 12;
	constexpr unsigned short ServerPort = 42405;
	constexpr int tickRate = 128;
	constexpr float threadSleepTime = (1.0f / static_cast<float>(tickRate)) * 1000.0f;
}

#endif
