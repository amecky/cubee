#pragma once
#include <base\GameState.h>
#include <renderer\render_types.h>
#include <ui\IMGUI.h>
#include "GameContext.h"

class GameOverState : public ds::GameState {

public:
	GameOverState(GameContext* ctx,ds::DialogManager* gui);
	virtual ~GameOverState() {}
	int update(float dt);
	void render();
	void activate();
	void deactivate();
private:
	GameContext* _context;
	ds::DialogManager* _gui;
};