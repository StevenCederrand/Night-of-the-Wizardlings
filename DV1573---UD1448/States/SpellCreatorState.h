#ifndef _SPELL_CREATOR_STATE_H
#define _SPELL_CREATOR_STATE_H
#include <Pch/Pch.h>
#include <System/State.h>

class SpellCreatorState : public State {

public:
	SpellCreatorState();
	virtual ~SpellCreatorState() override;
	virtual void update(float dt) override;
	virtual void render() override;


private:

};




#endif