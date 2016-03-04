#pragma once
#include <data\DynamicSettings.h>

struct GameSettings : public ds::DynamicGameSettings {

	float swapTTL;

	GameSettings() {
		addFloat("swap_ttl", &swapTTL, 1.0f);
	}

	const char* getFileName() const {
		return "game_settings.json";
	}

};
