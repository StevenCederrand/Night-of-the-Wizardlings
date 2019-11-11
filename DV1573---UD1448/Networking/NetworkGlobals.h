#ifndef _NETWORKGLOBALS_H
#define _NETWORKGLOBALS_H

namespace NetGlobals {

	constexpr unsigned int MaximumConnections = 6;
	constexpr unsigned short MaximumIncomingConnections = 6;
	constexpr unsigned short ServerPort = 42405;
	constexpr int tickRate = 128;
	constexpr unsigned int threadSleepTime = static_cast<unsigned int>((1.0f / static_cast<float>(tickRate)) * 1000u);
	constexpr unsigned short timeoutTimeMS = 5 * 1000;
	constexpr uint32_t serverCountdownTimeMS = 10 * 1000;
	constexpr uint32_t timeUntilRespawnMS = 5 * 1000;
	constexpr uint32_t roundTimeMS = 4 * 60 * 1000;
	constexpr uint32_t InGameEndStateTimeMS = 10 * 1000;
	constexpr uint32_t maxDelayBeforeDeletionMS = 20 * 1000;
	constexpr uint32_t pickupSpawnIntervalMS = 20 * 1000;
	constexpr uint32_t damageBuffActiveTimeMS = 10 * 1000;
	constexpr uint32_t pickupNotificationBeforeSpawnMS = 8.0f * 1000.0f;
	constexpr int maxPlayerHealth = 100;

	//static std::mutex gameSyncMutex;

	enum THREAD_FLAG {
		REMOVE,
		ADD,
		NONE
	};

	enum SERVER_STATE {
		WAITING_FOR_PLAYERS = 0,
		GAME_IS_STARTING,
		GAME_IN_SESSION,
		GAME_END_STATE
	};
}

#endif
