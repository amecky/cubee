#pragma once
#include "Constants.h"
#include <vector>
#include <sprites\Sprite.h>
#include "GameContext.h"
#include <utils\StateManager.h>

typedef ds::Array<ds::DroppedCell<GridEntry>> DroppedCells;
typedef ds::Array<ds::Point> Points;

enum BucketMode {
	B_IDLE,
	BK_INIT,
	BK_RUNNING,
	BK_MOVING,
	BK_REMOVING,
	BK_REFILLING,
	BK_SWAPPING,
	BK_BACK_SWAPPING,
	BK_DROPPING,
	BK_FILLRATE,
	BK_NONE
};

struct BucketContext : public ds::StateContext {
	ds::World* world;
	ColorGrid* grid;
	DroppedCells droppedCells;
	Points points;
	ds::Point firstSwapPoint;
	ds::Point secondSwapPoint;
	GameSettings* settings;
	Score* score;
};

class MouseOverState : public ds::State {

public:
	MouseOverState(ds::StateContext* context) : ds::State(context), _selectedEntry(-1, -1), _lastUpdate(-1, -1) {}
	virtual ~MouseOverState() {}
	int activate();
	int update(float dt);
	int getMode() const {
		return BK_RUNNING;
	}
	const char* getName() const {
		return "MouseOverState";
	}
private:
	ds::Point _selectedEntry;
	ds::Point _lastUpdate;
};

class SwapCellsState : public ds::State {

public:
	SwapCellsState(ds::StateContext* context) : ds::State(context) {}
	virtual ~SwapCellsState() {}
	int activate();
	int deactivate();
	int getMode() const {
		return BK_SWAPPING;
	}
	const char* getName() const {
		return "SwapCellsState";
	}
private:
	int findMatching(const ds::Point& p);
};

class SwapBackCellsState : public ds::State {

public:
	SwapBackCellsState(ds::StateContext* context) : ds::State(context) {}
	virtual ~SwapBackCellsState() {}
	int activate();
	int getMode() const {
		return BK_BACK_SWAPPING;
	}
	const char* getName() const {
		return "SwapBackCellsState";
	}
};

class RemoveCellsState : public ds::State {

public:
	RemoveCellsState(ds::StateContext* context) : ds::State(context) {}
	virtual ~RemoveCellsState() {}
	int activate();
	int deactivate();
	int getMode() const {
		return BK_REMOVING;
	}
	const char* getName() const {
		return "RemoveCellsState";
	}

};

class DropCellsState : public ds::State {

public:
	DropCellsState(ds::StateContext* context) : ds::State(context) {}
	virtual ~DropCellsState() {}
	int activate();
	int getMode() const {
		return BK_DROPPING;
	}
	const char* getName() const {
		return "DropCellsState";
	}
};

class RefillCellsState : public ds::State {

public:
	RefillCellsState(ds::StateContext* context);
	virtual ~RefillCellsState() {}
	int activate();
	int getMode() const {
		return BK_REFILLING;
	}
	const char* getName() const {
		return "RefillCellsState";
	}
private:
	void moveRow(int row);
	ds::SID _refill[GRID_SX];
};

class FillRateState : public ds::State {

public:
	FillRateState(ds::StateContext* context) : ds::State(context) {}
	virtual ~FillRateState() {}
	int activate();
	int getMode() const {
		return BK_FILLRATE;
	}
	const char* getName() const {
		return "FillRateState";
	}
};


// -------------------------------------------------------
// Bucket
// -------------------------------------------------------
class Bucket {

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
	void debug();
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
	const char* translateColor(int color) const;

	GameContext* _context;
	ds::World* _world;
	ds::Point _selectedEntry;
	ColorGrid m_Grid;
	PlayMode _playMode;
	BucketContext _bucketContext;
	ds::StateManager* _states;
};


