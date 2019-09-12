#include <Pch/Pch.h>
#include "StateManager.h"


StateManager::StateManager()
{
}


StateManager::~StateManager()
{
	clearStates();
}

void StateManager::popState()
{
	if (m_states.size() > 1) {
		delete m_states[m_states.size() - 1];
		m_states.pop_back();
	}
	else {
		logWarning("Trying to remove the only state left in the stack!");
	}
}

void StateManager::pushState(State* newState)
{
	newState->assignManager(this);
	m_states.emplace_back(newState);
	
}

void StateManager::clearAllAndSetState(State* newState)
{
	if (newState != nullptr) {
		clearStates();
		newState->assignManager(this);
		m_states.emplace_back(newState);
		
	}
	else {
		logError("Trying to push a new state which is a nullptr!");
	}
}

void StateManager::update(float dt)
{
	if (!m_states.empty()) {
		m_states.back()->update(dt);
	}
}

void StateManager::render()
{
	if (!m_states.empty()) {
		m_states.back()->render();
	}
}

void StateManager::clearStates()
{
	for (size_t i = 0; i < m_states.size(); i++) {
		delete m_states[i];
	}
	m_states.clear();
}