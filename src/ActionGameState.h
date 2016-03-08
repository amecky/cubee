#pragma once
#include <base\GameState.h>
#include <renderer\render_types.h>
#include "GameContext.h"
#include "Bucket.h"
#include "MainGameState.h"

class ActionGameState : public MainGameState {

public:
	ActionGameState(ds::GUIDialog* hud, GameContext* ctx);
	virtual ~ActionGameState();
	void tick(float dt);
	void activateHUD();
private:
	ds::GUIDialog* _hud;
	float _refillTimer;
};