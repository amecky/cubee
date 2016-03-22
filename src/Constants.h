#pragma once
#include <math\math_types.h>
#include <utils\Color.h>

const int GRID_SX = 10;
const int GRID_SY = 16;

const int START_X = 330;
const int START_Y = 128;
const int CELL_SIZE = 40;
const int MAX_COLORS = 4;
const int REFILL_Y_POS = 78;

const ds::Rect SELECTION_RECT = ds::Rect(80,240,40,40);

const int BLOCK_TOP = 0;
const int BLOCK_LEFT = 160;

const int OT_BORDER = 10;
const int OT_REFILL = 11;
const int OT_GRIDENTRY = 12;
	
const ds::Color BORDER_COLORS[] = {
	ds::Color( 72, 255, 0, 255),
	ds::Color(163, 209, 0, 255),
	ds::Color(255, 162, 0, 255),
	ds::Color(231, 116, 0, 255),
	ds::Color(209,  70, 0, 255)
};

enum BucketEvents {
	BE_NONE,
	BE_CALC_FILLRATE,
	BE_BUCKET_FULL,
	BE_EOL
};