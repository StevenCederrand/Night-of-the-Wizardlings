#ifndef _STATEMANAGER_H
#define _STATEMANAGER_H
#include <Pch/Pch.h>
#include "State.h"

class StateManager
{
public:
	StateManager();
	~StateManager();

	void popState();
	void pushState(State* newState);
	void clearAllAndSetState(State* state);

	void update(float dt);
	void render();

private:
	void clearStates();

private:
	std::vector<State*> m_states;
};

#endif