#pragma once
#include <base\GameState.h>
#include <renderer\render_types.h>
#include "GameContext.h"
#include "Bucket.h"
#include <dialogs\GUIDialog.h>
#include <effects\ScreenShakeEffect.h>

class MainGameState : public ds::GameState {

public:
	MainGameState(const char* name,GameContext* ctx);
	virtual ~MainGameState();
	int update(float dt);
	void render();
	void activate();
	void deactivate();
	int onButtonDown(int button, int x, int y);
	int onButtonUp(int button, int x, int y);
	int onChar(int ascii);
	virtual void tick(float dt) = 0;
	virtual void setScore(int points) = 0;
protected:
	virtual void activateHUD() = 0;
	virtual void deactivateHUD() = 0;
	
	GameContext* _context;
	float _timer;
	
	ds::ScreenShakeEffect* _effect;
	
};

