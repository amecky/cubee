#pragma once
#include "Constants.h"
#include <lib\Grid.h>
#include <vector>
#include <sprites\Sprite.h>
#include "GameContext.h"

struct Point {
	int x;
	int y;
	Point() : x(0), y(0) {}
	Point(int _x, int _y) : x(_x), y(_y) {}
};

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
// Bucket
// -------------------------------------------------------
class Bucket {

enum BucketMode {
	BK_RUNNING,
	BK_MOVING,
	BK_GLOWING,
	BK_REFILLING
};

typedef std::vector<ds::DroppedCell> RemovedCells;
typedef std::vector<ds::Sprite> Highlights;
typedef std::vector<ds::GridPoint> Points;

public:
	Bucket(GameContext* context);
	~Bucket();
	void init();
	void clear();
	void update(float elapsed);
	void drawGrid();
	void fillRow(int row,int pieces);
	void fill(int minCol,int maxCol);
	bool refill(int pieces,bool move = true);
	int selectCell(const Vector2f& mousePos);
	void moveRow(int row);
	bool isRowFull(int row);
	const int getOccupied() const {
		return m_PercentFilled;
	}
private:	
	const bool isValid(const Point& p) const;
	const bool isValid(int x, int y) const;
	const bool isUsed(int x, int y) const;
	int swapCells(int fx, int fy, int sx, int sy);
	int findMatching(int gx,int gy);
	void calculateFillRate();

	GameContext* _context;
	ds::World* _world;
	ds::SID _refill[GRID_SX];


	Highlights m_Highlights;
	Points m_Points;
	
	RemovedCells m_RemovedCells;
	ds::Sprite m_FirstSelection;
	ColorGrid m_Grid;
	BucketMode m_Mode;
	bool m_Refilling;
	float m_GlowTimer;
	int m_Filled;
	int m_PercentFilled;
	ds::Sprite m_BackGrid[GRID_SX * GRID_SY];
	ds::Sprite m_TopBar;
	ds::Sprite m_BottomBar;
};


