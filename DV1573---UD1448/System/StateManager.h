#ifndef _STATEMANAGER_H
#define _STATEMANAGER_H
#include <Pch/Pch.h>
#include "State.h"
#include <System/SoundHandler.h>


class StateManager
{
public:
	StateManager();
	~StateManager();

	void popState();
	void pushState(State* newState);
	void clearAllAndSetState(State* state);

	bool getImGuiState();
	void getGuiInfo();

	void update(float dt);
	void render();

private:
	void clearStates();
	void clearKillList();

private:
	std::vector<State*> m_states;
	std::vector<State*> m_killList;	
};

#endif