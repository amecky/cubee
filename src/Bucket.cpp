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
Bucket::Bucket(GameContext* context) : _context(context) , _world(context->world) , m_Grid(GRID_SX,GRID_SY) , _timer(0.0f) {		
	//clear();
}

Bucket::~Bucket() {}

// -------------------------------------------------------
// Init
// -------------------------------------------------------
void Bucket::init() {
	
	for ( int i = 0; i < GRID_SX; ++i ) {
		_refill[i] = _world->create(v2(START_X + i * CELL_SIZE, REFILL_Y_POS), ds::math::buildTexture(ds::Rect(BLOCK_TOP, BLOCK_LEFT, CELL_SIZE, CELL_SIZE)));
		_world->setColor(_refill[i], ds::Color(255, 255, 255, 128));
	}
	

	int i = 0;
	for ( int y = 0; y < GRID_SY; ++y ) {		
		_world->create(v2(512, START_Y + y * CELL_SIZE), ds::math::buildTexture(ds::Rect(0, 320, 440, 40)));
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
		entry.sid = _world->create(convert(row,y), ds::math::buildTexture(ds::Rect(BLOCK_TOP, BLOCK_LEFT + offset, CELL_SIZE, CELL_SIZE)));
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
			entry.sid = _world->create(convert(i, 0), ds::math::buildTexture(ds::Rect(BLOCK_TOP, BLOCK_LEFT + offset, CELL_SIZE, CELL_SIZE)));
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
void Bucket::update(float elapsed) {
	if (m_Mode == BK_RUNNING) {
		v2 mp = ds::renderer::getMousePosition();
		ds::Point p = convert(mp);
		if (isValid(p.x, p.y) && isUsed(p.x, p.y)) {
			if (p != _lastUpdate && p != _selectedEntry) {
				_lastUpdate = p;
				const GridEntry& entry = m_Grid.get(p.x, p.y);
				_world->startBehavior(entry.sid, "wiggle_scale");
			}
		}
	}
	else if (m_Mode == BK_GLOWING) {
		_timer += elapsed;
		if (_timer > FLASH_TTL) {
			debug();
			m_Mode = BK_MOVING;
			m_Grid.remove(m_Points);
			for (int i = 0; i < m_Points.size(); ++i) {				
				const GridEntry& e = m_Grid.get(m_Points[i]);
				LOG << i << " => removed " << m_Points[i].x << " " << m_Points[i].y << " sid: " << e.sid;
				_world->remove(e.sid);
			}
			_droppedCells.clear();
			m_Grid.dropCells(_droppedCells);
			for (size_t i = 0; i < _droppedCells.size(); ++i) {
				const ds::DroppedCell& dc = _droppedCells[i];				
				const GridEntry& org = m_Grid.get(dc.from.x, dc.from.y);
				LOG << i << " => dropped from " << dc.from.x << " " << dc.from.y << " to " << dc.to.x << " " << dc.to.y << " org: " << org.sid;
				v2 p = _world->getPosition(org.sid);
				_world->moveTo(org.sid, convert(dc.from.x,dc.from.y), convert(dc.to.x,dc.to.y), _context->settings->moveTTL);
			}
			_context->score.add(m_Points.size());
			_context->hud->setNumber(2, _context->score.points);
			m_Points.clear();
			_timer = 0.0f;
			debug();
		}
	}
	else if (m_Mode == BK_MOVING) {
		_timer += elapsed;
		if (_timer > _context->settings->moveTTL) {
			refill(GRID_SX);
			m_Mode = BK_REFILLING;
			_timer = 0.0f;
		}
	}
	else if (m_Mode == BK_REFILLING) {
		_timer += elapsed;
		if (_timer > FLASH_TTL) {
			m_Mode = BK_RUNNING;
			_timer = 0.0f;
		}
	}
	else if (m_Mode == BK_SWAPPING) {
		_timer += elapsed;
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
					_world->startBehavior(e.sid, "wiggle_scale");
				}
			}
		}
	}
	else if (m_Mode == BK_BACK_SWAPPING) {
		_timer += elapsed;
		if (_timer > _context->settings->swapTTL) {			
			m_Mode = BK_RUNNING;
			_timer = 0.0f;
		}
	}
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
		/*
		//const GridEntry& org = m_Grid(fx,fy);
		//m_Grid.set(fx,fy,m_Grid.get(sx,sy));
		//m_Grid.set(sx,sy,org);
		m_Points.clear();
		int total = findMatching(first);
		total += findMatching(second);
		if ( total == 0 ) {
			m_Grid.swap(second, first);
		}
		return total;
		*/
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
	LOG << "-------------------------------------------";
	for (int y = GRID_SY - 1; y >= 0; --y) {
		std::string tmp;
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
	LOG << "-------------------------------------------";
}