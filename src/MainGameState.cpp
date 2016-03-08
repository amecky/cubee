#include "MainGameState.h"


MainGameState::MainGameState(const char* name, GameContext* ctx) : ds::GameState(name), _context(ctx), _timer(0.0f) {
	_bucket = new Bucket(_context);
	_bucket->init();

}


MainGameState::~MainGameState() {
	delete _bucket;
}

int MainGameState::update(float dt) {
	_bucket->update(dt);
	tick(dt);
	//if ( m_Bucket.hasEvents()) {		
	//stopGame();
	//}
	return 0;
}

void MainGameState::render() {
	_context->world->render();
}

void MainGameState::activate() {
	_bucket->fill(2, 6);
	_bucket->refill(GRID_SX, false);
	activateHUD();
	_context->score.reset();
	_timer = 0.0f;
}

void MainGameState::deactivate() {

}

int MainGameState::onButtonDown(int button, int x, int y) {
	return 0;
}

int MainGameState::onButtonUp(int button, int x, int y) {
	int points = _bucket->selectCell();
	_context->score.add(points);
	/*
	m_HUD.setCounterValue(1,m_Score.moves);
	if ( points > 0 ) {
	if ( points > m_Score.bestMove ) {
	m_Score.bestMove = points;
	m_HUD.setCounterValue(2,m_Score.bestMove);
	}
	m_Score.points += points * 10;
	m_HUD.setCounterValue(0,m_Score.points);
	}
	*/
	return 0;
}

int MainGameState::onChar(int ascii) {
	if (ascii == 'r') {
		_bucket->refill(GRID_SX, true);
	}
	//if (ascii == 'e') {
		//stopGame();
	//}
	if (ascii == 'd') {
		_bucket->debug();
	}
	if (ascii == 't') {
		_bucket->toggleTimer();
	}
	if (ascii == 'k') {
		_bucket->kickTimer(3000.0f);
	}
	return 0;
}
