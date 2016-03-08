#pragma once
#include <world\World.h>
#include "GameSettings.h"

enum PlayMode {
	PM_ZEN,
	PM_ACTION,
	PM_EOL
};

struct Score {

	int points;
	int moves;
	int bestMove;

	Score() : points(0), moves(0), bestMove(0) {}

	void reset() {
		points = 0;
		moves = 0;
		bestMove = 0;
	}

	void add(int pieces) {
		points += pieces * 10;
		++moves;
		if (pieces > bestMove) {
			bestMove = pieces;
		}
	}
};

struct GameContext {

	ds::World* world;
	GameSettings* settings;
	Score score;
	PlayMode playMode;
};