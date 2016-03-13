#pragma once
#include <world\World.h>
#include "GameSettings.h"
#include <lib\Grid.h>

const ds::Point INVALID_POINT = ds::Point(-1, -1);

// -------------------------------------------------------
// GridEntry
// -------------------------------------------------------
struct GridEntry {
	ds::SID sid;
	int color;

	GridEntry() : sid(ds::INVALID_SID), color(0) {}
};

// -------------------------------------------------------
// ColorGrid
// -------------------------------------------------------
class ColorGrid : public ds::Grid<GridEntry> {

public:
	ColorGrid(int sizeX, int sizeY) : ds::Grid<GridEntry>(sizeX, sizeY) {}
	virtual ~ColorGrid() {}
	bool isMatch(const GridEntry& first, const GridEntry& right) {
		return first.color == right.color;
	}
};

// -------------------------------------------------------
// Play mode
// -------------------------------------------------------
enum PlayMode {
	PM_ZEN,
	PM_ACTION,
	PM_EOL
};

// -------------------------------------------------------
// Score
// -------------------------------------------------------
struct Score {

	int points;
	int moves;
	int bestMove;
	int filled;
	int percentFilled;

	Score() : points(0), moves(0), bestMove(0) {}

	void reset() {
		points = 0;
		moves = 0;
		bestMove = 0;
		filled = 0;
		percentFilled = 0;
	}

	void add(int pieces) {
		points += pieces * 10;
		++moves;
		if (pieces > bestMove) {
			bestMove = pieces;
		}
	}
};

class Bucket;
// -------------------------------------------------------
// GameContext
// -------------------------------------------------------
struct GameContext {

	ds::World* world;
	GameSettings* settings;
	Score score;
	PlayMode playMode;
	ds::SID bottomDivider;
	ds::SID topDivider;
	ds::SID leftBar;
	ds::SID rightBar;
	Bucket* bucket;
};