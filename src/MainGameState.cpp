#include "MainGameState.h"


MainGameState::MainGameState(const char* name, GameContext* ctx) : ds::GameState(name), _context(ctx), _timer(0.0f) {
	
	_effect = new ds::ScreenShakeEffect();
}


MainGameState::~MainGameState() {
	delete _effect;
}

int MainGameState::update(float dt) {
	_effect->tick(dt);

	int points = _context->bucket->update(dt);
	if (points > 0) {
		setScore(_context->score.points);
	}
	else if (points == -1) {
		// dead
		return 666;
	}
	tick(dt);
	return 0;
}

void MainGameState::render() {
	_effect->start();
	_context->world->render();
	_effect->render();
}

void MainGameState::activate() {
	_context->bucket->fill(2, 6);
	//_bucket->refill(GRID_SX, false);
	activateHUD();
	_context->score.reset();
	_timer = 0.0f;
}

void MainGameState::deactivate() {
	deactivateHUD();

}

int MainGameState::onButtonDown(int button, int x, int y) {
	return 0;
}

int MainGameState::onButtonUp(int button, int x, int y) {
	int points = _context->bucket->selectCell();
	
	return 0;
}

int MainGameState::onChar(int ascii) {
	if (ascii == 'r') {
		_context->bucket->refill(GRID_SX, true);
	}
	if (ascii == 'e') {
		return 666;
	}
	if (ascii == 'd') {
		_context->bucket->debug();
	}
	if (ascii == '1') {
		_effect->activate();
	}
	return 0;
}
