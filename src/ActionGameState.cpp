#include "ActionGameState.h"

ActionGameState::ActionGameState(ds::GUIDialog* hud, GameContext* ctx) : MainGameState("ActionGameState", ctx) , _hud(hud) {}

ActionGameState::~ActionGameState() {

}

void ActionGameState::tick(float dt) {
	_refillTimer += dt;
	if (_refillTimer > _context->settings->refillTimer) {
		_hud->resetTimer(5);
		_refillTimer = 0.0f;
		_bucket->refill(GRID_SX, true);
	}
}

void ActionGameState::activateHUD() {
	_hud->activate();
	_hud->resetTimer(3);
	_hud->startTimer(3);
	_hud->resetTimer(5);
	_hud->startTimer(5);
	_refillTimer = 0.0f;
}

void ActionGameState::deactivateHUD() {
	_hud->deactivate();
}

void ActionGameState::setScore(int points) {
	_hud->setNumber(2, points);
}