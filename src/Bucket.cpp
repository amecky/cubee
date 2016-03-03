#include "Bucket.h"
#include <math\GameMath.h>
#include <utils\Log.h>
#include <sprites\SpriteBatch.h>
#include <renderer\graphics.h>

v2 convert(int gx, int gy) {
	return v2(START_X + gx * CELL_SIZE, START_Y + gy * CELL_SIZE);
}

Point convert(const v2& screenPos) {
	int x = (screenPos.x - START_X + CELL_SIZE / 2) / CELL_SIZE;
	int y = (screenPos.y - START_Y + CELL_SIZE / 2) / CELL_SIZE;
	return Point(x, y);
}

// -------------------------------------------------------
// Grid
// -------------------------------------------------------
Bucket::Bucket(GameContext* context) : _context(context) , _world(context->world) , m_Grid(GRID_SX,GRID_SY) {		
	//clear();
}

Bucket::~Bucket() {}

// -------------------------------------------------------
// Init
// -------------------------------------------------------
void Bucket::init() {
	
	for ( int i = 0; i < GRID_SX; ++i ) {
		_refill[i] = _world->create(v2(START_X + i * CELL_SIZE, REFILL_Y_POS), ds::math::buildTexture(ds::Rect(BLOCK_TOP, 0, CELL_SIZE, CELL_SIZE)));
		_world->setColor(_refill[i], ds::Color(255, 255, 255, 64));
	}
	
	int i = 0;
	for ( int x = 0; x < GRID_SX; ++x ) {	
		for ( int y = 0; y < GRID_SY; ++y ) {			
			ds::Sprite* sp = &m_BackGrid[i];
			sp->position = v2(START_X + x * CELL_SIZE,START_Y + y * CELL_SIZE);
			sp->texture = ds::math::buildTexture(ds::Rect(0, 0, CELL_SIZE, CELL_SIZE));
			++i;
		}
	}
	m_BottomBar.position = v2(512,102);
	m_BottomBar.texture = ds::math::buildTexture(ds::Rect(190, 0, 400, 10));
	m_TopBar.position = v2(512,START_Y + GRID_SY * CELL_SIZE - 15);
	m_TopBar.texture = ds::math::buildTexture(ds::Rect(190, 0, 400, 10));

	_selection = _world->create(v2(START_X,START_Y),ds::math::buildTexture(SELECTION_RECT));
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
	_lastUpdate = Point(-1, -1);
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
}

// -------------------------------------------------------
// Fill row
// -------------------------------------------------------
void Bucket::fillRow(int row,int pieces) {
	for ( int y = 0; y < pieces; ++y ) {
		GridEntry entry;
		entry.color = ds::math::random(0, MAX_COLORS - 1);
		int offset = entry.color * CELL_SIZE;
		entry.sid = _world->create(v2(START_X + row * CELL_SIZE, START_Y + y * CELL_SIZE), ds::math::buildTexture(ds::Rect(BLOCK_TOP, offset, CELL_SIZE, CELL_SIZE)));
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
			_world->moveTo(entry.sid, s, e, 0.5f, 0, tweening::easeInOutQuad);
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
			entry.sid = _world->create(convert(i,0), ds::math::buildTexture(ds::Rect(BLOCK_TOP, offset, CELL_SIZE, CELL_SIZE)));
			m_Grid.set(i, 0, entry);
			v2 s = v2(START_X + i * CELL_SIZE, REFILL_Y_POS);
			v2 e = convert(i, 0);
			_world->moveTo(entry.sid, s, e, 0.5f, 0, tweening::easeInOutQuad);
			// if row is full we cannot refill and game is over
			if (isRowFull(i)) {
				LOG << "Row " << i << " is full";
				//RowFullEvent evn;
				//evn.row = i;
				//getEvents().add(1,&evn,sizeof(RowFullEvent));
				return false;
			}
			
			//m_MovingCells->addRefill(i,node->getID());		
		}
	}
	m_Mode = BK_REFILLING;
	for ( int i = 0; i < GRID_SX; ++i ) {
		int type = ds::math::random(0,MAX_COLORS-1);
		_world->setType(_refill[i], type);
		int offset = type * CELL_SIZE;
		_world->setTexture(_refill[i],ds::math::buildTexture(ds::Rect(120,offset,CELL_SIZE,CELL_SIZE)));
		v2 s = v2(START_X + i * CELL_SIZE, REFILL_Y_POS);
		v2 e = s;
		e.y -= 100.0f;
		_world->moveTo(_refill[i], e, s, 0.5f, 0, tweening::easeInOutQuad);
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
	v2 mp = ds::renderer::getMousePosition();
	Point p = convert(mp);
	if (isValid(p.x,p.y) && isUsed(p.x, p.y)) {		
		if (p.x != _lastUpdate.x || p.y != _lastUpdate.y) {
			_lastUpdate = p;
			const GridEntry& entry = m_Grid.get(p.x, p.y);
			_world->startBehavior(entry.sid, "wiggle_scale");
		}
	}
	if (m_Mode == BK_GLOWING) {
		m_GlowTimer += elapsed;
		if (m_GlowTimer > FLASH_TTL) {
			m_Mode = BK_MOVING;
			m_Grid.remove(m_Points);
			m_RemovedCells.clear();
			m_Highlights.clear();
			m_Grid.dropCells(m_RemovedCells);
			for (size_t i = 0; i < m_RemovedCells.size(); ++i) {
				const ds::DroppedCell& dc = m_RemovedCells[i];
				const GridEntry& org = m_Grid.get(dc.from.x, dc.from.y);
				v2 p = _world->getPosition(org.sid);
				_world->moveTo(org.sid, convert(dc.from.x,dc.from.y), convert(dc.to.x,dc.to.y), 0.4f);
				const GridEntry& old = m_Grid.get(dc.to.x, dc.to.y);
				_world->remove(old.sid);
			}
			m_Points.clear();
		}
	}
}
/*
void Bucket::update(float elapsed) {
	if ( m_Mode == BK_REFILLING ) {
		m_MovingCells->update(elapsed);
		if ( m_MovingCells->numActive() == 0 ) {
			m_Mode = BK_RUNNING;	
			calculateFillRate();
		}
	}
	if ( m_Mode == BK_MOVING ) {
		m_MovingCells->update(elapsed);
		if ( m_MovingCells->numActive() == 0 ) {
			refill(GRID_SX);
			m_Mode = BK_REFILLING;
		}
	}	

	
}
*/
// -------------------------------------------------------
// Draw grid
// -------------------------------------------------------
void Bucket::drawGrid() {
	int total = GRID_SX * GRID_SY;
	for ( int x = 0; x < total; ++x ) {	
		ds::sprites::draw(m_BackGrid[x]);		
	}
	ds::sprites::draw(m_TopBar);
	ds::sprites::draw(m_BottomBar);
}

// -------------------------------------------------------
// is valid position
// -------------------------------------------------------
const bool Bucket::isValid(const Point& p) const {
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
const bool Bucket::isUsed(const Point& p) const {
	return !m_Grid.isFree(p.x, p.y);
}

// -------------------------------------------------------
// Swap cells
// -------------------------------------------------------
int Bucket::swapCells(int fx,int fy,int sx,int sy) {	
	const GridEntry& org = m_Grid(fx,fy);
	m_Grid.set(fx,fy,m_Grid(sx,sy));
	m_Grid.set(sx,sy,org);
	//m_Highlights.clear();
	m_Points.clear();
	int total = findMatching(fx,fy);
	total += findMatching(sx,sy);	
	if ( total == 0 ) {		
		// not allowed - swap back
		const GridEntry& tmp = m_Grid(fx,fy);
		m_Grid.set(fx,fy,m_Grid(sx,sy));
		m_Grid.set(sx,sy,tmp);
	}
	return total;
}

int Bucket::findMatching(int gx,int gy) {
	int total = 0;
	ds::Array<ds::GridPoint> points;
	m_Grid.findMatchingNeighbours(gx,gy,points);
	if ( points.size() > 2 ) {
		total += points.size();
		for ( size_t i = 0; i < points.size(); ++i ) {
			const ds::GridPoint& gp = points[i];
			const GridEntry& c = m_Grid.get(gp.x,gp.y);
			ds::Sprite h;
			h.texture = ds::math::buildTexture(ds::Rect(80,300,58,58));
			h.position = v2(START_X + gp.x * CELL_SIZE,START_Y + gp.y * CELL_SIZE);					

			h.color = COLOR_ARRAY[c.color];
			m_Highlights.push_back(h);
			m_Points.push_back(points[i]);

			_world->scaleTo(c.sid, v2(1, 1), v2(0.1f, 0.1f), 0.2f);
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
	Point p = convert(mp);
	//if ( m_Mode == BK_RUNNING ) {		
		if ( isValid(p) && isUsed(p)) {
			if (_selectedEntry == INVALID_POINT) {
				_selectedEntry = p;
				// FIXME: show selection
				//m_FirstSelection.setPosition(Vector2f(START_X + x * CELL_SIZE,START_Y + y * CELL_SIZE));
				//m_FirstSelection.setTarget(Vector2f(x,y));
			}		
			else {
				ret = swapCells(p.x,p.y,_selectedEntry.x,_selectedEntry.y);
				if ( ret > 0 ) {
					m_GlowTimer = 0.0f;
					m_Mode = BK_GLOWING;
				}
				_selectedEntry = INVALID_POINT;
			}
		}
		else {
			_selectedEntry = INVALID_POINT;
		}
	//}
	//else {
		//LOG << "not running";
	//}
	return ret;
}
