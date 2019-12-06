#ifndef _PLAYER_EVENTS_H
#define _PLAYER_EVENTS_H

enum PlayerEvents {
	Died,
	Respawned,
	TookDamage,
	TookMana,
	TookHeal,
	SessionOver,
	Deflected,
	WallGotDestroyed,
	PlayerReady,
	GameStarted,
	GameEnded,
	Warmup,
	None,
	Hitmark,
	EnemyDeflected,
	GameCountdown,
	RoundTimer,
	GameIsAboutToStart,
	WaitingForPlayers

};

struct Evnt
{
	PlayerEvents playerEvent;
	void* data;
};

#endif
