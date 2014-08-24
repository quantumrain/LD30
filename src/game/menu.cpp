#include "Pch.h"
#include "Common.h"
#include "Game.h"

bool menu_update() {
	if (gPressedSelect) {
		gPressedSelect = false;
		return true;
	}

	return false;
}

void menu_render(draw_context* dc) {
	draw_string(dc, vec2(320.0f, 40.0f), 2.0f, TEXT_CENTRE, colours::SILVER, "YOU'RE ONLY MAKING IT WORSE");

	float y = 80.0f;

	draw_string(dc, vec2(320.0f, y), 1.5f, TEXT_CENTRE, lerp(colour(colours::BLUE), colour(), 0.5f), "Shoot blue things"); y += 15.0f;
	draw_string(dc, vec2(320.0f, y), 1.5f, TEXT_CENTRE, lerp(colour(colours::RED), colour(), 0.5f), "Avoid red things"); y += 15.0f;
	draw_string(dc, vec2(320.0f, y), 1.5f, TEXT_CENTRE, lerp(colour(colours::GREEN), colour(), 0.5f), "Collect green things"); y += 15.0f;

	draw_string(dc, vec2(320.0f, 320.0f), 2.0f, TEXT_CENTRE | TEXT_BOTTOM, colours::SILVER, "press fire");
}