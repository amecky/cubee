#include "Cubee.h"
#include "utils\Log.h"
#include "GameContext.h"
#include <base\GameStateMachine.h>
#include "ZenGameState.h"
#include "ActionGameState.h"
#include "GameOverState.h"

ds::BaseApp *app = new Cubee(); 

Cubee::Cubee() : ds::BaseApp() {
	//_CrtSetBreakAlloc(5376);
}

// -------------------------------------------------------
// Load content and prepare game
// -------------------------------------------------------
bool Cubee::loadContent() {	
	_context = new GameContext();
	_context->world = world;
	_context->settings = new GameSettings();
	_context->settings->load();
	//_context->hud = gui->get("HUD");
	addGameState(new ActionGameState(gui->get("ACTION_HUD"), _context));
	addGameState(new ZenGameState(gui->get("ZEN_HUD"), _context));
	addGameState(new ds::BasicMenuGameState("MainMenuState", "MainMenu", gui));
	addGameState(new GameOverState(_context, gui));
	connectGameStates("ActionGameState", 666, "GameOverState");
	connectGameStates("ZenGameState", 666, "GameOverState");
	connectGameStates("GameOverState", 1, "MainMenuState");
	return true;
}

void Cubee::init() {
	activate("MainMenuState");
}

void Cubee::onGUIButton( ds::DialogID dlgID,int button ) {
	LOG << "dialog " << dlgID << " button " << button;
	if ( dlgID == 2 && button == 0 ) {
		_context->playMode = PM_ZEN;
		activate("ZenGameState");
	}
	if (dlgID == 2 && button == 4) {
		_context->playMode = PM_ACTION;
		activate("ActionGameState");
	}
	else if ( dlgID == 2 && button == 1 ) {
		shutdown();
	}
}
