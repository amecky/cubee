#include "ZenGameState.h"


ZenGameState::ZenGameState(ds::GUIDialog* hud,GameContext* ctx) : MainGameState("ZenGameState",ctx) , _hud(hud) {}

ZenGameState::~ZenGameState() {}

void ZenGameState::tick(float dt) {
}


void ZenGameState::activateHUD() {
	_hud->activate();
	_hud->resetTimer(3);
	_hud->startTimer(3);
	_count = 0;
}

void ZenGameState::deactivateHUD() {
	_hud->deactivate();
}

void ZenGameState::setScore(int points) {
	_context->score.add(points);
	_hud->setNumber(2, _context->score.points);
}