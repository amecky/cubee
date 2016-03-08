#include "ZenGameState.h"


ZenGameState::ZenGameState(ds::GUIDialog* hud,GameContext* ctx) : MainGameState("ZenGameState",ctx) , _hud(hud) {}

ZenGameState::~ZenGameState() {}

void ZenGameState::tick(float dt) {
	if (_diff > 0) {
		_scoreTimer += dt;
		if (_scoreTimer > 0.1f) {
			--_diff;
			_context->score.add(10);
			_hud->setNumber(2, _context->score.points);
			_scoreTimer -= 0.1f;
		}
	}
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
	//_hud->setNumber(2, points);
	LOG << "==> points: " << points;
	_diff = points;
	_scoreTimer = 0.0f;
}