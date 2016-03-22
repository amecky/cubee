#include "Cubee.h"
#include "utils\Log.h"
#include "GameContext.h"
#include <base\GameStateMachine.h>
#include "ZenGameState.h"
#include "ActionGameState.h"
#include "GameOverState.h"
#include <renderer\TGAImage.h>
#include <stdint.h>

ds::BaseApp *app = new Cubee(); 

Cubee::Cubee() : ds::BaseApp() {
	//_CrtSetBreakAlloc(5376);
}

Cubee::~Cubee() {
	delete _context->settings;
	delete _context->bucket;
	delete _context;
}

// -------------------------------------------------------
// Load content and prepare game
// -------------------------------------------------------
bool Cubee::loadContent() {	
	_context = new GameContext();
	_context->world = world;
	_context->settings = new GameSettings();
	_context->settings->load();
	addGameState(new ActionGameState(gui->get("ACTION_HUD"), _context));
	addGameState(new ZenGameState(gui->get("ZEN_HUD"), _context));
	addGameState(new ds::BasicMenuGameState("MainMenuState", "MainMenu", gui));
	addGameState(new GameOverState(_context, gui));
	connectGameStates("ActionGameState", 666, "GameOverState");
	connectGameStates("ZenGameState", 666, "GameOverState");
	connectGameStates("GameOverState", 1, "MainMenuState");

	//ds::TGAImage t;
	//t.read_tga_file("content\\textures\\TextureArray.tga");

	_context->leftBar = world->create(v2(294, 430), ds::math::buildTexture(0, 840, 6, 640));
	_context->rightBar = world->create(v2(730, 430), ds::math::buildTexture(0, 840, 6, 640));
	int i = 0;
	for (int y = 0; y < GRID_SY; ++y) {
		world->create(v2(512, START_Y + y * CELL_SIZE), ds::math::buildTexture(ds::Rect(0, 340, 448, 40)), OT_BORDER);
	}
	_context->bottomDivider = world->create(v2(512, 102), ds::math::buildTexture(250, 0, 400, 4));
	_context->topDivider = world->create(v2(512, 754), ds::math::buildTexture(250, 0, 400, 4));

	_context->bucket = new Bucket(_context);
	_context->bucket->init();

	uint8_t uc = 127;
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
