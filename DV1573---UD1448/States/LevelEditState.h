#ifndef _LEVEL_EDIT_STATE_H
#define _LEVEL_EDIT_STATE_H
#include <Pch/Pch.h>
#include <System/State.h>
#include <Gui/Gui.h>
#include <HUD/HudHandler.h>


class LevelEditState : public State
{
public:
	LevelEditState();
	virtual void update(float dt) override;
	virtual void render() override;

private:
	void onSpellHit_callback(); //<--- This might not be needed
	void update_isPlaying(const float& dt);
	void update_isSpectating(const float& dt);
};



#endif