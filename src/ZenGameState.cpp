#include "ZenGameState.h"


ZenGameState::ZenGameState(ds::GUIDialog* hud,GameContext* ctx) : MainGameState("ZenGameState",ctx) , _hud(hud) {}


void ZenGameState::tick(float dt) {
}


void ZenGameState::activateHUD() {
	_hud->activate();
	_hud->resetTimer(3);
	_hud->startTimer(3);
}