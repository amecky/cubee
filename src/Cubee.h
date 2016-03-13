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

public:	
	Cubee();
	virtual ~Cubee();
	bool loadContent();
	const char* getTitle() {
		return "Cubee";
	}
	void init();
private:
	void onGUIButton( ds::DialogID dlgID,int button );
	GameContext* _context;
};