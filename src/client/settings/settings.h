#pragma once

#include <raylib.h>

struct Settings
{
	Color color {PINK};
	float scale {1.5f};
	float delay {0.0f};
	float speed {2.0f};
	bool isSitting {false};
};

Settings loadSettings();
void saveSettings(Settings& settings);
