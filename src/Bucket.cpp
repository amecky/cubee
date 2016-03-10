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
// Grid
// -------------------------------------------------------
Bucket::Bucket(GameContext* context) : _context(context) , _world(context->world) , m_Grid(GRID_SX,GRID_SY) , _timer(0.0f) , _useTimer(true) , _playMode(PM_ZEN) {		
	//clear();
}

Bucket::~Bucket() {}

void Bucket::toggleTimer() {
	_useTimer = !_useTimer;
}
// -------------------------------------------------------
// Init
// -------------------------------------------------------
void Bucket::init() {
	
	for ( int i = 0; i < GRID_SX; ++i ) {
		_refill[i] = _world->create(v2(START_X + i * CELL_SIZE, REFILL_Y_POS), ds::math::buildTexture(ds::Rect(BLOCK_TOP, BLOCK_LEFT, CELL_SIZE, CELL_SIZE)),OT_REFILL);
		_world->setColor(_refill[i], ds::Color(255, 255, 255, 128));
	}
	

	int i = 0;
	for ( int y = 0; y < GRID_SY; ++y ) {		
		_world->create(v2(512, START_Y + y * CELL_SIZE), ds::math::buildTexture(ds::Rect(0, 320, 440, 40)),OT_BORDER);
	}
	
	//_bottomBar = _world->create(v2(512, 102), ds::math::buildTexture(240, 0, 100, 6));

	m_TopBar.position = v2(512,START_Y + GRID_SY * CELL_SIZE - 15);
	m_TopBar.texture = ds::math::buildTexture(ds::Rect(190, 0, 400, 10));

	//_selection = _world->create(v2(START_X,START_Y),ds::math::buildTexture(SELECTION_RECT));
	_selectedEntry = INVALID_POINT;
}

// -------------------------------------------------------
// Clear grid
// -------------------------------------------------------
void Bucket::clear() {
	for ( int x = 0; x < GRID_SX; ++x ) {
		if (_refill[x] != ds::INVALID_SID) {
			_world->remove(_refill[x]);
		}
	}
	// FIXME: remove everything from _world
	m_Grid.clear();
	//m_FirstSelection.setActive(false);
	m_Mode = BK_RUNNING;
	m_Filled = 0;
	_lastUpdate = INVALID_POINT;
}

// -------------------------------------------------------
// Fill entire grid
// -------------------------------------------------------

void Bucket::fill(int minCol,int maxCol) {
	for ( int x = 0; x < GRID_SX; ++x ) {
		int pieces = ds::math::random(minCol,maxCol);
		fillRow(x,pieces);
	}
	calculateFillRate();
	m_Mode = BK_RUNNING;
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
// Move row upwards
// -------------------------------------------------------
void Bucket::moveRow(int row) {
	for ( int y = GRID_SY - 1; y > 0; --y ) {
		if ( !m_Grid.isFree(row,y-1) ) {
			const GridEntry& entry = m_Grid.get(row, y);
			m_Grid.set(row,y,m_Grid.get(row,y-1));
			m_Grid.remove(row,y-1);		
			v2 s = convert(row, y - 1);
			v2 e = convert(row, y);
			if (_world->contains(entry.sid)) {
				_world->moveTo(entry.sid, s, e, _context->settings->moveTTL, 0, tweening::easeInOutQuad);
			}
		}
	}
}

// -------------------------------------------------------
// Refill
// -------------------------------------------------------
bool Bucket::refill(int pieces,bool move) {
	// move current refill nodes to nodes
	if ( move ) {
		for ( int i = 0; i < GRID_SX; ++i ) {
			moveRow(i);
			ds::SID node = _refill[i];
			int type = _world->getType(node);
			GridEntry entry;
			entry.color = _world->getType(node);
			int offset = entry.color * CELL_SIZE;
			entry.sid = _world->create(convert(i, 0), ds::math::buildTexture(ds::Rect(BLOCK_TOP, BLOCK_LEFT + offset, CELL_SIZE, CELL_SIZE)), OT_GRIDENTRY);
			m_Grid.set(i, 0, entry);
			v2 s = v2(START_X + i * CELL_SIZE, REFILL_Y_POS);
			v2 e = convert(i, 0);
			_world->moveTo(entry.sid, s, e, _context->settings->moveTTL, 0, tweening::easeInOutQuad);
			// if row is full we cannot refill and game is over
			if (isRowFull(i)) {
				LOG << "Row " << i << " is full";
				return false;
			}
		}
		m_Mode = BK_REFILLING;
	}	
	for ( int i = 0; i < GRID_SX; ++i ) {
		int type = ds::math::random(0,MAX_COLORS-1);
		_world->setType(_refill[i], type);
		int offset = type * CELL_SIZE;
		_world->setTexture(_refill[i], ds::math::buildTexture(ds::Rect(BLOCK_TOP, BLOCK_LEFT + offset, CELL_SIZE, CELL_SIZE)));
		v2 s = v2(START_X + i * CELL_SIZE, REFILL_Y_POS);
		v2 e = s;
		e.y -= 100.0f;
		_world->moveTo(_refill[i], e, s, _context->settings->moveTTL, 0, tweening::easeInOutQuad);
	}
	return true;
}
// -------------------------------------------------------
// Calculate fill rate
// -------------------------------------------------------
void Bucket::calculateFillRate() {
	m_Filled = 0;
	for ( int x = 0; x < GRID_SX; ++x ) {
		for ( int y = 0; y < GRID_SY; ++y ) {			
			if ( !m_Grid.isFree(x,y)) {
				++m_Filled;
			}
		}
	}
	LOG << "filled " << m_Filled;
	float percentage = static_cast<float>(m_Filled) / (static_cast<float>(GRID_SX) * static_cast<float>(GRID_SY)) * 100.0f;
	LOG << "percentage " << percentage;
	m_PercentFilled = static_cast<int>(percentage);
}



// -------------------------------------------------------
// Update
// -------------------------------------------------------
int Bucket::update(float elapsed) {

	if (_useTimer) {
		ds::renderer::print(v2(10, 10), "Timer: ON", ds::Color::BLACK);
	}
	else {
		ds::renderer::print(v2(10, 10), "Timer: OFF", ds::Color::BLACK);
	}
	ds::renderer::print(v2(10, 30), translate(m_Mode), ds::Color::BLACK);

	if (m_Mode == BK_RUNNING) {
		v2 mp = ds::renderer::getMousePosition();
		ds::Point p = convert(mp);
		if (isValid(p.x, p.y) && isUsed(p.x, p.y)) {
			if (p != _lastUpdate && p != _selectedEntry) {
				_lastUpdate = p;
				const GridEntry& entry = m_Grid.get(p.x, p.y);
				if (_world->contains(entry.sid)) {
					_world->startBehavior(entry.sid, "wiggle_scale");
				}
				else {
					LOG << "INVALID sid: " << p.x << " " << p.y << " " << entry.sid;
				}
			}
		}
		return 0;
	}
	else if (m_Mode == BK_GLOWING) {
		if (_useTimer) {
			_timer += elapsed;
		}
		if (_timer > _context->settings->moveTTL) {
			m_Mode = BK_DROPPING;
			_timer = 0.0f;
			m_Grid.remove(m_Points,false);
			for (int i = 0; i < m_Points.size(); ++i) {
				const GridEntry& e = m_Grid.get(m_Points[i]);
				LOG << i << " => removed " << m_Points[i].x << " " << m_Points[i].y << " sid: " << e.sid;
				_world->remove(e.sid);
			}
		}
		return 0;
	}
	else if (m_Mode == BK_DROPPING) {
		if (_useTimer) {
			_timer += elapsed;
		}
		if (_timer > _context->settings->moveTTL) {
			m_Mode = BK_MOVING;
			_droppedCells.clear();
			m_Grid.dropCells(_droppedCells);
			for (size_t i = 0; i < _droppedCells.size(); ++i) {
				const ds::DroppedCell<GridEntry>& dc = _droppedCells[i];				
				LOG << i << " => dropped from " << dc.from.x << " " << dc.from.y << " to " << dc.to.x << " " << dc.to.y << " org: " << dc.data.sid;
				v2 p = _world->getPosition(dc.data.sid);
				_world->moveTo(dc.data.sid, convert(dc.from.x,dc.from.y), convert(dc.to.x,dc.to.y), _context->settings->moveTTL);
			}
			int points = m_Points.size();
			m_Points.clear();
			_timer = 0.0f;
			return points;
		}
	}
	else if (m_Mode == BK_MOVING) {
		if (_useTimer) {
			_timer += elapsed;
		}
		if (_timer > _context->settings->moveTTL) {
			if (!refill(GRID_SX)) {
				return -1;
			}
			m_Mode = BK_REFILLING;
			_timer = 0.0f;
		}
		return 0;
	}
	else if (m_Mode == BK_REFILLING) {
		if (_useTimer) {
			_timer += elapsed;
		}
		if (_timer > _context->settings->moveTTL) {
			m_Mode = BK_RUNNING;
			_timer = 0.0f;
			debug();
			synch();
		}
		return 0;
	}
	else if (m_Mode == BK_SWAPPING) {
		if (_useTimer) {
			_timer += elapsed;
		}
		if (_timer > _context->settings->swapTTL) {
			m_Points.clear();
			int total = findMatching(_firstSwapPoint);
			total += findMatching(_secondSwapPoint);
			if ( total == 0 ) {
				m_Mode = BK_BACK_SWAPPING;
				const GridEntry& f = m_Grid.get(_firstSwapPoint);
				const GridEntry& s = m_Grid.get(_secondSwapPoint);
				_world->moveTo(f.sid, convert(_firstSwapPoint), convert(_secondSwapPoint), _context->settings->swapTTL, 0, tweening::linear);
				_world->moveTo(s.sid, convert(_secondSwapPoint), convert(_firstSwapPoint), _context->settings->swapTTL, 0, tweening::linear);
				m_Grid.swap(_firstSwapPoint, _secondSwapPoint);
			}
			else {
				_timer = 0.0f;
				m_Mode = BK_GLOWING;
				for (int i = 0; i < m_Points.size(); ++i) {
					const GridEntry& e = m_Grid.get(m_Points[i]);
					_world->startBehavior(e.sid, "fade_scale");
				}
			}
		}
		return 0;
	}
	else if (m_Mode == BK_BACK_SWAPPING) {
		if (_useTimer) {
			_timer += elapsed;
		}
		if (_timer > _context->settings->swapTTL) {			
			m_Mode = BK_RUNNING;
			_timer = 0.0f;
		}
		return 0;
	}
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
		const GridEntry& f = m_Grid.get(first);
		const GridEntry& s = m_Grid.get(second);
		_world->moveTo(f.sid, convert(first), convert(second), _context->settings->swapTTL, 0, tweening::linear);
		_world->moveTo(s.sid, convert(second), convert(first), _context->settings->swapTTL, 0, tweening::linear);
		m_Grid.swap(first, second);
		m_Mode = BK_SWAPPING;
		_timer = 0.0f;
		_firstSwapPoint = first;
		_secondSwapPoint = second;
	}
	return 0;
}

// -------------------------------------------------------
// find matching
// -------------------------------------------------------
int Bucket::findMatching(const ds::Point& p) {
	int total = 0;
	ds::Array<ds::Point> points;
	m_Grid.findMatchingNeighbours(p.x,p.y,points);
	if ( points.size() > 2 ) {
		total += points.size();
		for ( size_t i = 0; i < points.size(); ++i ) {
			const ds::Point& gp = points[i];
			const GridEntry& c = m_Grid.get(gp.x,gp.y);
			m_Points.push_back(points[i]);
			_world->scaleTo(c.sid, v2(1, 1), v2(0.1f, 0.1f), 0.4f);
		}
	}
	return total;
}

// -------------------------------------------------------
// Select cell
// -------------------------------------------------------
int Bucket::selectCell() {
	int ret = -1;
	v2 mp = ds::renderer::getMousePosition();
	ds::Point p = convert(mp);
	if ( m_Mode == BK_RUNNING ) {		
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
				if ( ret > 0 ) {
					_timer = 0.0f;
					m_Mode = BK_GLOWING;
				}
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
			if (m_Grid.isFree(x, y)) {
				sprintf_s(buffer, 32, "-------- ");
			}
			else {
				const GridEntry& e = m_Grid.get(x, y);
				sprintf_s(buffer, 32, "%2d (%3d) ", e.color, e.sid);				
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

const char* Bucket::translate(BucketMode mode) {
	switch (mode) {
		case BK_RUNNING: return "Running"; break;
		case BK_MOVING: return "Moving"; break;
		case BK_GLOWING: return "Glowing"; break;
		case BK_REFILLING: return "Refilling"; break;
		case BK_SWAPPING: return "Swapping"; break;
		case BK_BACK_SWAPPING: return "BackSwapping"; break;
		case BK_DROPPING: return "Dropping"; break;
		default: return "UNKNOWN";
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