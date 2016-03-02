#pragma comment(lib, "Diesel2D.lib")
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "dxerr.lib")
#pragma warning(disable : 4995)

#pragma once
#include "base\BaseApp.h"
#include "dxstdafx.h"
#include <renderer\render_types.h>
#include "Bucket.h"

struct GameContext;

class Cubee : public ds::BaseApp {

enum GameMode {
	GM_START,
	GM_RUNNING,
	GM_OVER
};

struct Score {

	int points;
	int moves;
	int bestMove;
	int occupied;

};

public:	
	Cubee();
	virtual ~Cubee() {
		delete _bucket;
		delete _context;
	}	
	bool loadContent();
	const char* getTitle() {
		return "Cubee";
	}
	void init();
	void update(float dt);
	void draw();
	virtual void OnChar( char ascii,unsigned int keyState );
	virtual void OnButtonUp( int button,int x,int y );
private:
	void stopGame();
	void restart();
	void onGUIButton( ds::DialogID dlgID,int button );
	ds::Sprite m_Background;
	
	float m_Timer;
	GameMode m_Mode;
	Score m_Score;

	GameContext* _context;
	Bucket* _bucket;
};