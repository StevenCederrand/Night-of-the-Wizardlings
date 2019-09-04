#ifndef _STATE_H
#define _STATE_H
#include "PersistentData.h"
class StateManager; // Empty

class State {

public:
	virtual ~State() {}
	virtual void update(float dt) = 0;
	virtual void render() = 0;

	/*
		This is set by the StateManager class
		so that the actual state can change to another state.
		For an example, menustate wants to change to playstate. 
		Because the statemanager handles the state change the state
		needs to have a reference of it.
	*/
	inline void assignManager(StateManager* manager) {
		m_stateManager = manager;
	}

	/*
		This is also set by the StateManager class.
		So you should just be able to use it by typing:
		m_pd->...
		Read what persistenData is in the PersistentData.h
	*/
	inline void assignPersistentData(PersistentData* pd) {
		m_pd = pd;
	}

protected:
	StateManager* m_stateManager;
	PersistentData* m_pd;
};


#endif