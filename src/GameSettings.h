#pragma once
#include <data\DynamicSettings.h>

struct GameSettings : public ds::DynamicGameSettings {

	float swapTTL;
	float moveTTL;
	float refillTTL;
	float dropTTL;
	float removeTTL;
	float refillTimer;

	GameSettings() {
		addFloat("swap_ttl", &swapTTL, 1.0f);
		addFloat("move_ttl", &moveTTL, 1.0f);
		addFloat("refill_ttl", &refillTTL, 1.0f);
		addFloat("drop_ttl", &dropTTL, 1.0f);
		addFloat("remove_ttl", &removeTTL, 1.0f);
		addFloat("refill_timer", &refillTimer, 10.0f);
	}

	const char* getFileName() const {
		return "game_settings.json";
	}

};
