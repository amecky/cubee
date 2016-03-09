#include "GameOverState.h"
#include "utils\Log.h"
#include <renderer\shader.h>
#include <renderer\BitmapFont.h>
#include <math\GameMath.h>
#include <utils\Profiler.h>

GameOverState::GameOverState(GameContext* ctx, ds::DialogManager* gui) : ds::GameState("GameOverState") , _context(ctx) , _gui(gui) {}

// -------------------------------------------------------
// Update
// -------------------------------------------------------
int GameOverState::update(float dt) {
	return 0;
}

// -------------------------------------------------------
// Draw
// -------------------------------------------------------
void GameOverState::render() {
	
}

void GameOverState::activate() {
	_gui->activate("GameOver");
	// points 13
	// time 16
	// best move 19
	// total 22
}

void GameOverState::deactivate() {
	_gui->deactivate("GameOver");
}
