#include "menu.h"
#include <raylib.h>
#include <raygui.h>
#include "../settings/settings.h"



bool Menu::isToggled()
{
	return m_menuToggle;
}

void Menu::drawOnClose(UserMenu& userMenu)
{
        DrawRectangleRounded(
		userMenu.rectangle,
		userMenu.cornerRadius, userMenu.cornerSegments,
		userMenu.color
	);
}

void Menu::drawOnOpen(UserMenu& userMenu, Settings& settings, float screenWidth)
{
        DrawRectangleRounded(
		userMenu.rectangle,
		userMenu.cornerRadius,
		userMenu.cornerSegments,
		userMenu.color
	);

	DrawRectangleRounded(
		{screenWidth/2 - 30, 10.0f, 60.0f, 20.0f},
		0.4,
		10,
		{159, 25, 58, 255}
	);

	DrawText("CLOSE", screenWidth/2 - 23, 15.0f, 13, RAYWHITE);

	GuiSlider(
		(Rectangle){
			userMenu.rectangle.x + 50,
			userMenu.rectangle.y + 40,
			userMenu.rectangle.width - 100,
			userMenu.rectangle.y
		},
		"- Size",
		"+ Size",
		&settings.scale,
		0.5f,
		4.5f
	);

	GuiSlider(
		(Rectangle){
			userMenu.rectangle.x + 50,
			userMenu.rectangle.y + 70,
			userMenu.rectangle.width - 100,
			userMenu.rectangle.y
		},
		"- Speed",
		"+ Speed",
		&settings.speed,
		0.5f,
		4.5f
	);

	GuiSlider(
		(Rectangle){
			userMenu.rectangle.x + 50,
			userMenu.rectangle.y + 90,
			userMenu.rectangle.width - 100,
			userMenu.rectangle.y
		},
		"- Delay",
		"+ Delay",
		&settings.delay,
		0.0f,
		7.0f
	);


	GuiColorPicker(
		(Rectangle){
			userMenu.rectangle.x + 50,
			userMenu.rectangle.y + 110,
			80,
			userMenu.rectangle.y + 60
		},
		"Test Picker",
		&settings.color
	);


	GuiCheckBox(
		(Rectangle){
			userMenu.rectangle.width + userMenu.rectangle.x - 70,
			userMenu.rectangle.y + 110,
			20,
			userMenu.rectangle.y
		},
		"Sit",
		&settings.isSitting
	);


}

#define HOVERED_MENU_COLOR 240, 231, 216, 255
#define ORIGINAL_MENU_COLOR 80, 101, 104, 150

void Menu::update(const Vector2& mousePos, UserMenu &userMenu, float screenWidth, Settings& settings)
{
        m_onMenuHover = CheckCollisionPointRec(
		{mousePos.x, mousePos.y},
		{screenWidth/2 - 30, 10.0f, 60.0f, 20.0f}
        );


	const bool isClicked = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);

	if(m_menuToggle)
        {
		// Toggle Menu On
		ClearWindowState(FLAG_WINDOW_MOUSE_PASSTHROUGH);
		userMenu.color = {HOVERED_MENU_COLOR};

		if(isClicked && m_onMenuHover) // Close Menu
		{

			m_menuToggle = false;
			userMenu.rectangle.x = userMenu.rectangle.x + 180;

			userMenu.rectangle.width = m_closeMenu[WIDTH];
			userMenu.rectangle.height = m_closeMenu[HEIGHT];

			userMenu.cornerRadius = m_closeMenu[RADIUS];
			userMenu.cornerSegments = m_closeMenu[SEGMENTS];

			SetWindowState(FLAG_WINDOW_MOUSE_PASSTHROUGH);
			userMenu.color = {ORIGINAL_MENU_COLOR};
		}
	}

	else if (m_onMenuHover) 
	{


		ClearWindowState(FLAG_WINDOW_MOUSE_PASSTHROUGH);
		userMenu.color = {HOVERED_MENU_COLOR};

		if (isClicked) // Open Menu
		{
			m_menuToggle = true;
			userMenu.rectangle.x = userMenu.rectangle.x - 180;

			userMenu.rectangle.width = m_openMenu[WIDTH];
			userMenu.rectangle.height = m_openMenu[HEIGHT];

			userMenu.cornerRadius = m_openMenu[RADIUS];
			userMenu.cornerSegments = m_openMenu[SEGMENTS];

			SetWindowState(FLAG_WINDOW_MOUSE_PASSTHROUGH);
			userMenu.color = {ORIGINAL_MENU_COLOR};
		}

	}

	else
	{
		SetWindowState(FLAG_WINDOW_MOUSE_PASSTHROUGH);
		userMenu.color = {ORIGINAL_MENU_COLOR};
	}


}


