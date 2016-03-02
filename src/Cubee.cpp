#include "Cubee.h"
#include "utils\Log.h"
#include "GameContext.h"

ds::BaseApp *app = new Cubee(); 

Cubee::Cubee() : ds::BaseApp() {
	//_CrtSetBreakAlloc(5376);
	m_Timer = 0.0f;
	m_Mode = GM_START;	
}

// -------------------------------------------------------
// Load content and prepare game
// -------------------------------------------------------
bool Cubee::loadContent() {	
	_context = new GameContext();
	_context->world = world;
	_bucket = new Bucket(_context);
	//m_Background.setPosition(Vector2f(512,384));
	//m_Background.setTextureRect(ds::Rect(0,512,512,384));
	//m_Background.setScale(Vector2f(2.0f,2.0f));	
	//createGameObject<Bucket>(&m_Bucket);	
	//gui.activate("Highscores");	
	return true;
}

void Cubee::init() {
	_bucket->init();
	restart();
}

// -------------------------------------------------------
// Restart game
// -------------------------------------------------------
void Cubee::restart() {
	/*
	m_Bucket.clear();
	m_Score.points = 0;
	m_Score.moves = 0;
	m_Score.occupied = 0;
	m_Score.bestMove = 0;
	*/
	_bucket->fill(2, 6);
	_bucket->refill(GRID_SX, false);
	m_Mode = GM_RUNNING;
	/*
	m_HUD.setCounterValue(0,0);
	m_HUD.setCounterValue(1,0);
	m_HUD.setCounterValue(2,0);
	m_HUD.setTimer(0,0,0);
	std::string str;
	ds::string::formatInt(m_Bucket.getOccupied(),str);
	m_HUD.setText(8,str+"%");
	*/
}
// -------------------------------------------------------
// Update
// -------------------------------------------------------
void Cubee::update(float dt) {
	_bucket->update(dt);
	//if ( m_Bucket.hasEvents()) {		
		//stopGame();
	//}
	if ( m_Mode == GM_RUNNING ) {
		/*
		int occupied = m_Bucket.getOccupied();
		if ( occupied != m_Score.occupied ) {
			m_Score.occupied = occupied;
			std::string str;
			ds::string::formatInt(m_Score.occupied,str);
			m_HUD.setText(8,str+"%");
		}
		*/
	}
}

// -------------------------------------------------------
// Draw
// -------------------------------------------------------
void Cubee::draw() {	
	//renderer->draw(m_Background);	
	if ( m_Mode == GM_RUNNING ) {	
		_context->world->render();
		//m_Bucket.drawGrid();
		//m_Bucket.render();
		//m_HUD.render();
	}
}

// -------------------------------------------------------
// OnChar
// -------------------------------------------------------
void Cubee::OnChar( char ascii,unsigned int keyState ) {
	if ( ascii == 'r' ) {
		_bucket->refill(GRID_SX, true);
	}
	if ( ascii == 'e' ) {
		stopGame();
	}
}

// -------------------------------------------------------
// Stop game
// -------------------------------------------------------
void Cubee::stopGame() {
	m_Mode = GM_OVER;
	/*
	gui.activate("GameOver");
	ds::GUIDialog* dlg = gui.get("GameOver");
	std::string str;
	ds::string::formatInt(m_Score.points,str,6);
	dlg->updateText(1,str);
	ds::string::formatInt(m_Score.moves,str);
	dlg->updateText(5,str);
	ds::string::formatInt(m_Score.bestMove,str);
	dlg->updateText(7,str);
	ds::GameTimer* timer = m_HUD.getTimer(0);
	ds::string::formatTime(timer->getMinutes(),timer->getSeconds(),str);
	dlg->updateText(3,str);
	*/
}

// -------------------------------------------------------
// OnButtonUp
// -------------------------------------------------------
void Cubee::OnButtonUp( int button,int x,int y ) {
	if ( m_Mode == GM_RUNNING ) {
		int points = _bucket->selectCell(Vector2f(x, y));
		++m_Score.moves;
		/*
		m_HUD.setCounterValue(1,m_Score.moves);
		if ( points > 0 ) {
			if ( points > m_Score.bestMove ) {
				m_Score.bestMove = points;
				m_HUD.setCounterValue(2,m_Score.bestMove);
			}
			m_Score.points += points * 10;	
			m_HUD.setCounterValue(0,m_Score.points);
		}
		*/
	}
}

void Cubee::onGUIButton( ds::DialogID dlgID,int button ) {
	LOG << "dialog " << dlgID << " button " << button;
	/*
	if ( dlgID == 0 && button == 0 ) {
		gui.deactivate("GameOver");
		restart();
	}
	else if ( dlgID == 0 && button == 1 ) {
		gui.deactivate("GameOver");
		gui.activate("MainMenu");
	}
	else if ( dlgID == 1 && button == 0 ) {
		gui.deactivate("MainMenu");
		restart();
	}
	else if ( dlgID == 1 && button == 3 ) {
		gui.activate("Credits");
		gui.deactivate("MainMenu");
	}
	else if ( dlgID == 1 && button == 2 ) {
		gui.activate("Highscores");
		gui.deactivate("MainMenu");
	}
	else if ( dlgID == 1 && button == 1 ) {
		shutdown();
	}
	else if ( dlgID == 2 && button == 1 ) {
		gui.deactivate("Credits");
		gui.activate("MainMenu");
	}
	else if ( dlgID == 3 && button == 1 ) {
		gui.deactivate("Highscores");
		gui.activate("MainMenu");
	}
	*/
}
