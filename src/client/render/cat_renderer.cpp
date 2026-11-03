#include "cat_renderer.h"
#include <raylib.h>

namespace
{
	void drawTexture(const Texture2D& currentFrame, Vector2 position, Color catColor, float scale)
	{
		DrawTextureEx(currentFrame, position, 0.0f, scale, catColor);
	}
}

void drawCat(const CatState &state, GameState &game, float scale)
{
	Texture2D frame = game.frameFor(state);
	drawTexture(frame, {state.x, state.y}, GetColor(state.color), scale);
}

