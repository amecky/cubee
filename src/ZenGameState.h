#pragma once
#include <base\GameState.h>
#include <renderer\render_types.h>
#include "GameContext.h"
#include "Bucket.h"
#include "MainGameState.h"

class ZenGameState : public MainGameState {

public:
	ZenGameState(ds::GUIDialog* hud,GameContext* ctx);
	virtual ~ZenGameState();
	void tick(float dt);
	void activateHUD();
private:
	ds::GUIDialog* _hud;
};

