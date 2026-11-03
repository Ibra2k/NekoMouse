#pragma once

#include <array>
#include <cstdint>
#include <raylib.h>
#include "../settings/settings.h"

struct UserMenu 
{
	Rectangle rectangle {};
	float cornerRadius {};
	int cornerSegments {};
	Color color {};
};



enum MenuValues : uint8_t
{
	WIDTH,
	HEIGHT,
	RADIUS,
	SEGMENTS,
	TOTAL,
};


class Menu
{
	bool m_onMenuHover {};

	bool m_menuToggle {};

	std::array<float, TOTAL> m_openMenu {420.0f, 200.0f, 0.4f, 10};
	std::array<float, TOTAL> m_closeMenu {60.0f, 20.0f, 1.0f, 10};

public:


	Menu ()
	{}

	bool isToggled();
	void drawOnClose(UserMenu& userMenu);
	void drawOnOpen(UserMenu& userMenu, Settings& settings, float screenWidth);
	void update(const Vector2& mousePos, UserMenu& userMenu, float screenWidth, Settings& settings);

};
