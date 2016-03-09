#pragma once
#include "Constants.h"
#include <lib\Grid.h>
#include <vector>
#include <sprites\Sprite.h>
#include "GameContext.h"



const ds::Point INVALID_POINT = ds::Point(-1, -1);

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
	BK_REFILLING,
	BK_SWAPPING,
	BK_BACK_SWAPPING,
	BK_DROPPING
};

typedef ds::Array<ds::DroppedCell<GridEntry>> DroppedCells;
typedef ds::Array<ds::Point> Points;

public:
	Bucket(GameContext* context);
	~Bucket();
	void init();
	void clear();
	int update(float elapsed);
	void fillRow(int row,int pieces);
	void fill(int minCol,int maxCol);
	bool refill(int pieces,bool move = true);
	int selectCell();
	void moveRow(int row);
	bool isRowFull(int row);
	const int getOccupied() const {
		return m_PercentFilled;
	}
	void debug();
	void toggleTimer();
	void kickTimer(float v) {
		_timer += v;
	}
	void synch();
private:	
	bool Bucket::containsGridEntry(ds::SID sid);
	const bool isValid(const ds::Point& p) const;
	const bool isValid(int x, int y) const;
	const bool isUsed(int x, int y) const;
	const bool isUsed(const ds::Point& p) const;
	int swapCells(const ds::Point& first, const ds::Point& second);
	int findMatching(const ds::Point& p);
	void calculateFillRate();
	const char* translate(BucketMode mode);

	GameContext* _context;
	ds::World* _world;
	ds::SID _refill[GRID_SX];
	Points m_Points;
	DroppedCells _droppedCells;	
	ds::Point _selectedEntry;
	ds::Point _lastUpdate;
	float _timer;
	float _currentTTL;
	ds::SID _selection;
	ds::Point _firstSwapPoint;
	ds::Point _secondSwapPoint;
	bool _useTimer;

	ColorGrid m_Grid;
	BucketMode m_Mode;
	bool m_Refilling;
	//float m_GlowTimer;
	int m_Filled;
	int m_PercentFilled;
	ds::Sprite m_TopBar;
	ds::SID _bottomBar;
	PlayMode _playMode;
};


