#include "Bucket.h"
#include <math\GameMath.h>
#include <utils\Log.h>
#include <sprites\SpriteBatch.h>
#include <renderer\graphics.h>

v2 convert(int gx, int gy) {
	return v2(START_X + gx * CELL_SIZE, START_Y + gy * CELL_SIZE);
}

v2 convert(const ds::Point& p) {
	return v2(START_X + p.x * CELL_SIZE, START_Y + p.y * CELL_SIZE);
}

ds::Point convert(const v2& screenPos) {
	int x = (screenPos.x - START_X + CELL_SIZE / 2) / CELL_SIZE;
	int y = (screenPos.y - START_Y + CELL_SIZE / 2) / CELL_SIZE;
	return ds::Point(x, y);
}

// -------------------------------------------------------
// Mouse over state
// -------------------------------------------------------
int MouseOverState::activate() {
	_selectedEntry = INVALID_POINT;
	_lastUpdate = INVALID_POINT;
	return 0;
}

int MouseOverState::update(float dt) {
	BucketContext* ctx = static_cast<BucketContext*>(_ctx);
	v2 mp = ds::renderer::getMousePosition();
	ds::Point p = convert(mp);
	if (ctx->grid->isValid(p.x, p.y) && !ctx->grid->isFree(p.x, p.y)) {
		if (p != _lastUpdate && p != _selectedEntry) {
			_lastUpdate = p;
			const GridEntry& entry = ctx->grid->get(p.x, p.y);
			if (ctx->world->contains(entry.sid)) {
				ctx->world->startBehavior(entry.sid, "wiggle_scale");
			}
			else {
				LOG << "INVALID sid: " << p.x << " " << p.y << " " << entry.sid;
			}
		}
	}
	return 0;
}

// -------------------------------------------------------
// swap cells state
// -------------------------------------------------------
int SwapCellsState::activate() {
	BucketContext* ctx = static_cast<BucketContext*>(_ctx);
	const GridEntry& f = ctx->grid->get(ctx->firstSwapPoint);
	const GridEntry& s = ctx->grid->get(ctx->secondSwapPoint);
	LOG << "swapping first: " << ctx->firstSwapPoint.x << " " << ctx->firstSwapPoint.y << " second: " << ctx->secondSwapPoint.x << " " << ctx->secondSwapPoint.y;
	ctx->world->moveTo(f.sid, convert(ctx->firstSwapPoint), convert(ctx->secondSwapPoint), ctx->settings->swapTTL, 0, tweening::linear);
	ctx->world->moveTo(s.sid, convert(ctx->secondSwapPoint), convert(ctx->firstSwapPoint), ctx->settings->swapTTL, 0, tweening::linear);
	ctx->grid->swap(ctx->firstSwapPoint, ctx->secondSwapPoint);
	return 0;
}

// -------------------------------------------------------
// find matching
// -------------------------------------------------------
int SwapCellsState::findMatching(const ds::Point& p) {
	BucketContext* ctx = static_cast<BucketContext*>(_ctx);
	int total = 0;
	ctx->points.clear();
	ds::Array<ds::Point> points;
	ctx->grid->findMatchingNeighbours(p.x, p.y, points);
	if (points.size() > 2) {
		total += points.size();
		for (size_t i = 0; i < points.size(); ++i) {
			const ds::Point& gp = points[i];
			const GridEntry& c = ctx->grid->get(gp.x, gp.y);
			ctx->points.push_back(points[i]);
		}
	}
	LOG << "found " << total << " matching points at " << p.x << " " << p.y;
	return total;
}

int SwapCellsState::deactivate() {
	BucketContext* ctx = static_cast<BucketContext*>(_ctx);
	int firstMatches = findMatching(ctx->firstSwapPoint);
	int secondMatches = findMatching(ctx->secondSwapPoint);
	LOG << "first matches: " << firstMatches << " second matches: " << secondMatches;
	if (firstMatches < 2 || secondMatches < 2) {
		return 1;
	}
	return 0;
}

int SwapBackCellsState::activate() {
	BucketContext* ctx = static_cast<BucketContext*>(_ctx);
	const GridEntry& f = ctx->grid->get(ctx->firstSwapPoint);
	const GridEntry& s = ctx->grid->get(ctx->secondSwapPoint);
	ctx->world->moveTo(f.sid, convert(ctx->firstSwapPoint), convert(ctx->secondSwapPoint), ctx->settings->swapTTL, 0, tweening::linear);
	ctx->world->moveTo(s.sid, convert(ctx->secondSwapPoint), convert(ctx->firstSwapPoint), ctx->settings->swapTTL, 0, tweening::linear);
	ctx->grid->swap(ctx->firstSwapPoint, ctx->secondSwapPoint);
	return 0;
}

// -------------------------------------------------------
// remove cells state
// -------------------------------------------------------
int RemoveCellsState::activate() {
	BucketContext* ctx = static_cast<BucketContext*>(_ctx);
	for (size_t i = 0; i < ctx->points.size(); ++i) {
		const ds::Point& gp = ctx->points[i];
		const GridEntry& c = ctx->grid->get(gp.x, gp.y);		
		ctx->world->scaleTo(c.sid, v2(1, 1), v2(0.1f, 0.1f), 0.4f);
	}
	return 0;
}

int RemoveCellsState::deactivate() {
	BucketContext* ctx = static_cast<BucketContext*>(_ctx);
	ctx->grid->remove(ctx->points, false);
	for (int i = 0; i < ctx->points.size(); ++i) {
		const GridEntry& e = ctx->grid->get(ctx->points[i]);
		LOG << i << " => removed " << DBG_PNT(ctx->points[i]) << " sid: " << e.sid;
		ctx->world->remove(e.sid);
	}
	return 0;
}

// -------------------------------------------------------
// drop cells state
// -------------------------------------------------------
int DropCellsState::activate() {
	BucketContext* ctx = static_cast<BucketContext*>(_ctx);
	ctx->droppedCells.clear();
	ctx->grid->dropCells(ctx->droppedCells);
	for (size_t i = 0; i < ctx->droppedCells.size(); ++i) {
		const ds::DroppedCell<GridEntry>& dc = ctx->droppedCells[i];
		LOG << i << " => dropped from " << dc.from.x << " " << dc.from.y << " to " << dc.to.x << " " << dc.to.y << " org: " << dc.data.sid;
		v2 p = ctx->world->getPosition(dc.data.sid);
		ctx->world->moveTo(dc.data.sid, convert(dc.from.x, dc.from.y), convert(dc.to.x, dc.to.y), ctx->settings->moveTTL);
	}
	return 0;
}

// -------------------------------------------------------
// refill cells state
// -------------------------------------------------------
RefillCellsState::RefillCellsState(ds::StateContext* context) : ds::State(context) {
	BucketContext* ctx = static_cast<BucketContext*>(_ctx);
	for (int i = 0; i < GRID_SX; ++i) {
		int type = ds::math::random(0, MAX_COLORS - 1);		
		int offset = type * CELL_SIZE;
		_refill[i] = ctx->world->create(v2(START_X + i * CELL_SIZE, REFILL_Y_POS), ds::math::buildTexture(ds::Rect(BLOCK_TOP, BLOCK_LEFT + offset, CELL_SIZE, CELL_SIZE)), OT_REFILL);
		ctx->world->setType(_refill[i], type);
		ctx->world->setColor(_refill[i], ds::Color(255, 255, 255, 128));
	}
}

// -------------------------------------------------------
// Move row upwards
// -------------------------------------------------------
void RefillCellsState::moveRow(int row) {
	BucketContext* ctx = static_cast<BucketContext*>(_ctx);
	for (int y = GRID_SY - 1; y > 0; --y) {
		if (!ctx->grid->isFree(row, y - 1)) {
			const GridEntry& entry = ctx->grid->get(row, y);
			ctx->grid->set(row, y, ctx->grid->get(row, y - 1));
			ctx->grid->remove(row, y - 1);
			v2 s = convert(row, y - 1);
			v2 e = convert(row, y);
			if (ctx->world->contains(entry.sid)) {
				ctx->world->moveTo(entry.sid, s, e, ctx->settings->moveTTL, 0, tweening::easeInOutQuad);
			}
		}
	}
}

int RefillCellsState::activate() {	
	BucketContext* ctx = static_cast<BucketContext*>(_ctx);
	for (int i = 0; i < GRID_SX; ++i) {
		moveRow(i);
		ds::SID node = _refill[i];
		int type = ctx->world->getType(node);
		GridEntry entry;
		entry.color = ctx->world->getType(node);
		XASSERT(entry.color < MAX_COLORS, "Color out of range: %d", entry.color);
		int offset = entry.color * CELL_SIZE;
		entry.sid = ctx->world->create(convert(i, 0), ds::math::buildTexture(ds::Rect(BLOCK_TOP, BLOCK_LEFT + offset, CELL_SIZE, CELL_SIZE)), OT_GRIDENTRY);
		ctx->grid->set(i, 0, entry);
		v2 s = v2(START_X + i * CELL_SIZE, REFILL_Y_POS);
		v2 e = convert(i, 0);
		ctx->world->moveTo(entry.sid, s, e, ctx->settings->moveTTL, 0, tweening::easeInOutQuad);
	}
	for (int i = 0; i < GRID_SX; ++i) {
		int type = ds::math::random(0, MAX_COLORS - 1);
		ctx->world->setType(_refill[i], type);
		int offset = type * CELL_SIZE;
		ctx->world->setTexture(_refill[i], ds::math::buildTexture(ds::Rect(BLOCK_TOP, BLOCK_LEFT + offset, CELL_SIZE, CELL_SIZE)));
		v2 s = v2(START_X + i * CELL_SIZE, REFILL_Y_POS);
		v2 e = s;
		e.y -= 100.0f;
		ctx->world->moveTo(_refill[i], e, s, ctx->settings->moveTTL, 0, tweening::easeInOutQuad);
	}
	return 0;
}


int FillRateState::activate() {
	BucketContext* ctx = static_cast<BucketContext*>(_ctx);
	ctx->score->filled = 0;
	for (int x = 0; x < GRID_SX; ++x) {
		for (int y = 0; y < GRID_SY; ++y) {
			if (!ctx->grid->isFree(x, y)) {
				++ctx->score->filled;
			}
		}
	}
	LOG << "filled " << ctx->score->filled;
	float percentage = static_cast<float>(ctx->score->filled) / (static_cast<float>(GRID_SX)* static_cast<float>(GRID_SY)) * 100.0f;
	LOG << "percentage " << percentage;
	int lookup = percentage / 20;
	ctx->world->setColor(_context->leftBar, BORDER_COLORS[lookup]);
	ctx->world->setColor(_context->rightBar, BORDER_COLORS[lookup]);
	ctx->score->percentFilled = static_cast<int>(percentage);
	// find max height
	// set texture for left bar
	int mc = ctx->grid->getMaxColumn() + 1;
	if (mc > GRID_SY) {
		mc = GRID_SY;
	}
	int th = CELL_SIZE * mc;
	ctx->world->setTexture(_context->leftBar, ds::math::buildTexture(0, 840, 6, th));
	ctx->world->setTexture(_context->rightBar, ds::math::buildTexture(0, 840, 6, th));
	// set position for left bar
	v2 org = v2(294, 430);
	int yp = 110 + th / 2;
	LOG << "mc: " << mc << " th: " << th << " yp: " << yp;
	ctx->world->setPosition(_context->leftBar, v2(294, yp));
	ctx->world->setPosition(_context->rightBar, v2(730, yp));
	return 0;
}

// -------------------------------------------------------
// Bucket
// -------------------------------------------------------
Bucket::Bucket(GameContext* context) : _context(context) , _world(context->world) , m_Grid(GRID_SX,GRID_SY) , _playMode(PM_ZEN) {		
	//clear();
	_bucketContext.world = _world;
	_bucketContext.grid = &m_Grid;
	_bucketContext.settings = _context->settings;
	_bucketContext.score = &_context->score;
	_states = new ds::StateManager(&_bucketContext);
	_states->add<MouseOverState>();
	_states->add<SwapCellsState>();
	_states->add<SwapBackCellsState>();
	_states->add<RemoveCellsState>();
	_states->add<DropCellsState>();
	_states->add<RefillCellsState>();
	_states->add<FillRateState>();
	_states->addTransition(BK_SWAPPING, 1, BK_BACK_SWAPPING, 0.5f);
	_states->addTransition(BK_SWAPPING, 0, BK_REMOVING, 0.5f);
	_states->addTransition(BK_REMOVING, 0, BK_DROPPING, 0.5f);
	_states->addTransition(BK_BACK_SWAPPING, 0, BK_RUNNING, 0.5f);
	_states->addTransition(BK_DROPPING, 0, BK_REFILLING, 0.5f);
	_states->addTransition(BK_REFILLING, 0, BK_FILLRATE, 0.5f);
	_states->addTransition(BK_FILLRATE, 0, BK_RUNNING, 0.0f);
}

Bucket::~Bucket() {
	delete _states;
}

// -------------------------------------------------------
// Init
// -------------------------------------------------------
void Bucket::init() {
	
}

// -------------------------------------------------------
// Clear grid
// -------------------------------------------------------
void Bucket::clear() {
	// FIXME: remove everything from _world
	m_Grid.clear();
}

// -------------------------------------------------------
// Fill entire grid
// -------------------------------------------------------

void Bucket::fill(int minCol,int maxCol) {
	for ( int x = 0; x < GRID_SX; ++x ) {
		int pieces = ds::math::random(minCol,maxCol - 1);
		fillRow(x,pieces);
	}
	_selectedEntry = INVALID_POINT;
	calculateFillRate();
	_states->activate(BK_RUNNING);
}

// -------------------------------------------------------
// Fill row
// -------------------------------------------------------
void Bucket::fillRow(int row,int pieces) {
	for ( int y = 0; y < pieces; ++y ) {
		GridEntry entry;
		entry.color = ds::math::random(0, MAX_COLORS - 1);
		int offset = entry.color * CELL_SIZE;
		v2 pos = convert(row, y);
		entry.sid = _world->create(pos, ds::math::buildTexture(ds::Rect(BLOCK_TOP, BLOCK_LEFT + offset, CELL_SIZE, CELL_SIZE)),OT_GRIDENTRY);
		v2 sp = pos;
		sp.y += 800.0f + CELL_SIZE * y;
		_world->moveTo(entry.sid, sp, pos, 0.8f);
		m_Grid.set(row,y,entry);
	}
}
// -------------------------------------------------------
// Is row full
// -------------------------------------------------------
bool Bucket::isRowFull(int row) {
	int cnt = 0;
	for ( int y = 0; y < GRID_SY; ++y ) {
		if ( m_Grid.isFree(row,y) ) {
			++cnt;
		}
	}
	return cnt == 0;
}


// -------------------------------------------------------
// Refill
// -------------------------------------------------------
bool Bucket::refill(int pieces,bool move) {
	_states->activate(BK_REFILLING);
	return true;
}
// -------------------------------------------------------
// Calculate fill rate
// -------------------------------------------------------
void Bucket::calculateFillRate() {
	
}



// -------------------------------------------------------
// Update
// -------------------------------------------------------
int Bucket::update(float elapsed) {
	_states->tick(elapsed);
	return 0;
}

// -------------------------------------------------------
// is valid position
// -------------------------------------------------------
const bool Bucket::isValid(const ds::Point& p) const {
	if (p.x < 0 || p.x >= GRID_SX) {
		return false;
	}
	if (p.y < 0 || p.y >= GRID_SY) {
		return false;
	}
	return true;
}

// -------------------------------------------------------
// is valid position
// -------------------------------------------------------
const bool Bucket::isValid(int x,int y) const {
	if ( x < 0 || x >= GRID_SX ) {
		return false;
	}
	if ( y < 0 || y >= GRID_SY ) {
		return false;
	}
	return true;
}

// -------------------------------------------------------
// is used
// -------------------------------------------------------
const bool Bucket::isUsed(int x,int y) const {
	return !m_Grid.isFree(x,y);	
}

// -------------------------------------------------------
// is used
// -------------------------------------------------------
const bool Bucket::isUsed(const ds::Point& p) const {
	return !m_Grid.isFree(p.x, p.y);
}

// -------------------------------------------------------
// Swap cells
// -------------------------------------------------------
int Bucket::swapCells(const ds::Point& first, const ds::Point& second) {
	if (first != second) {		
		_bucketContext.firstSwapPoint = first;
		_bucketContext.secondSwapPoint = second;
		_states->activate(BK_SWAPPING);
	}
	return 0;
}

// -------------------------------------------------------
// Select cell
// -------------------------------------------------------
int Bucket::selectCell() {
	int ret = -1;
	v2 mp = ds::renderer::getMousePosition();
	ds::Point p = convert(mp);
	if ( _states->getCurrentMode() == BK_RUNNING ) {		
		if ( isValid(p) && isUsed(p)) {
			if (_selectedEntry == INVALID_POINT) {
				_selectedEntry = p;
				const GridEntry& entry = m_Grid.get(p);
				_world->scale(entry.sid, 1.2f, 1.2f);				
			}	
			else if (_selectedEntry == p) {
				const GridEntry& entry = m_Grid.get(_selectedEntry);
				_world->scale(entry.sid, 1.0f, 1.0f);
				_selectedEntry = INVALID_POINT;
			}
			else {
				const GridEntry& entry = m_Grid.get(_selectedEntry);
				_world->scale(entry.sid, 1.0f, 1.0f);
				ret = swapCells(p,_selectedEntry);
				LOG << "--------> RET: " << ret;
				_selectedEntry = INVALID_POINT;
			}
		}
		else {
			_selectedEntry = INVALID_POINT;
		}
	}
	else {
		LOG << "not running";
	}
	return ret;
}

void Bucket::debug() {
	char buffer[32];
	LOG << "------------------------------------------------------------------------------------------------";
	for (int y = GRID_SY - 1; y >= 0; --y) {
		std::string tmp;
		sprintf(buffer,"%2d | ", y);
		tmp += buffer;
		for (int x = 0; x < GRID_SX; ++x) {
			const GridEntry& e = m_Grid.get(x, y);
			if (m_Grid.isFree(x, y)) {
				sprintf_s(buffer, 32, "-- (%3d) ",e.sid);
			}
			else {
				//const GridEntry& e = m_Grid.get(x, y);
				sprintf_s(buffer, 32, "%s (%3d) ", translateColor(e.color), e.sid);				
			}
			tmp += buffer;
		}
		LOG << tmp;
	}
	LOG << "------------------------------------------------------------------------------------------------";
	std::string tmp = "     ";
	for (int x = 0; x < GRID_SX; ++x) {
		sprintf_s(buffer, 32, "  %2d     ", x);
		tmp += buffer;
	}
	LOG << tmp;	
}

const char* Bucket::translateColor(int color) const {
	switch (color) {
		case 0: return "RD"; break;
		case 1: return "TQ"; break;
		case 2: return "YL"; break;
		case 3: return "GR"; break;
		default: return "UK"; break;
	}
}



bool Bucket::containsGridEntry(ds::SID sid) {
	for (int x = 0; x < GRID_SX; ++x) {
		for (int y = 0; y < GRID_SY; ++y) {
			if (!m_Grid.isFree(x, y)) {
				const GridEntry& entry = m_Grid.get(x, y);
				if (entry.sid == sid) {
					return true;
				}
			}
		}
	}
	return false;
}

void Bucket::synch() {
	for (int x = 0; x < GRID_SX; ++x) {
		for (int y = 0; y < GRID_SY; ++y) {
			if (!m_Grid.isFree(x, y)) {
				const GridEntry& entry = m_Grid.get(x, y);
				if (!_world->contains(entry.sid)) {
					LOG << "invalid grid entry at : " << x << " " << y << " sid: " << entry.sid;
				}
			}
		}
	}
	ds::SID ids[256];
	int num = _world->find_by_type(OT_GRIDENTRY,ids, 256);
	for (int i = 0; i < num; ++i) {
		if (!containsGridEntry(ids[i])) {
			LOG << "found sprite not in bucket: " << ids[i];
		}
	}
}