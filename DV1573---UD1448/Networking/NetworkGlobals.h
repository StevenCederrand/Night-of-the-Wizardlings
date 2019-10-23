#ifndef _NETWORKGLOBALS_H
#define _NETWORKGLOBALS_H

namespace NetGlobals {

	constexpr unsigned int MaximumConnections = 12;
	constexpr unsigned short MaximumIncomingConnections = 12;
	constexpr unsigned short ServerPort = 42405;
	constexpr int tickRate = 128;
	constexpr unsigned int threadSleepTime = static_cast<unsigned int>((1.0f / static_cast<float>(tickRate)) * 1000u);
	constexpr unsigned short timeoutTimeMS = 5 * 1000;
	constexpr uint32_t serverCountdownTimeMS = 5 * 1000;
	constexpr uint32_t timeUntilRespawnMS = 3 * 1000;
	constexpr uint32_t roundTime = 2 * 60 * 1000;
	constexpr int maxPlayerHealth = 100;

	static std::mutex gameSyncMutex;

	enum THREAD_FLAG {
		REMOVE,
		ADD,
		NONE
	};

	enum SERVER_STATE {
		WAITING_FOR_PLAYERS = 0,
		GAME_IS_STARTING,
		GAME_IN_SESSION,
	};
}

#endif
