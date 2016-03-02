#pragma once
#include <math\math_types.h>
#include <utils\Color.h>

const int GRID_SX = 10;
const int GRID_SY = 16;

const int START_X = 332;
const int START_Y = 128;

const int CELL_SIZE = 40;

const int MAX_COLORS = 4;

const int REFILL_Y_POS = 78;

const float REFILL_TTL = 1.0f;

const float MOVE_TTL = 0.25f;

const float FLASH_TTL = 0.35f;

const ds::Rect SELECTION_RECT = ds::Rect(80,240,40,40);

const int BLOCK_TOP = 120;

const ds::Color COLOR_ARRAY[] = {
	ds::Color(162,14,14,255) ,
	ds::Color(215,215,215,255),
	ds::Color(75,75,75,255),
	ds::Color(198,0,131,255),
	ds::Color(82,102,90,255)
};

struct RowFullEvent {

	int row;

};

