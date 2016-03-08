#include "ActionGameState.h"

ActionGameState::ActionGameState(ds::GUIDialog* hud, GameContext* ctx) : MainGameState("ActionGameState", ctx) , _hud(hud) {}


void ActionGameState::tick(float dt) {
}

void ActionGameState::activateHUD() {
	_hud->activate();
	_hud->resetTimer(3);
	_hud->startTimer(3);
}