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
	draw_string(dc, vec2(320.0f, 30.0f), 2.5f, TEXT_CENTRE, colours::ORANGE, "LETHAL REFLECTION");
	draw_string(dc, vec2(320.0f, 50.0f), 0.75f, TEXT_CENTRE, colours::ORANGE * 0.5f, "created for ludum dare 30 by stephen cakebread @quantumrain");

	float y = 85.0f;

	draw_string(dc, vec2(320.0f, y), 1.5f, TEXT_CENTRE, lerp(colour(colours::BLUE), colour(), 0.5f), "shoot blue things"); y += 15.0f;
	draw_string(dc, vec2(320.0f, y), 1.5f, TEXT_CENTRE, lerp(colour(colours::RED), colour(), 0.5f), "avoid red things"); y += 15.0f;
	draw_string(dc, vec2(320.0f, y), 1.5f, TEXT_CENTRE, lerp(colour(colours::GREEN), colour(), 0.5f), "collect green things"); y += 15.0f;

	float x0 = 320.0f - 140.0f;
	float x1 = 320.0f + 140.0f;

	y += 35.0f;

	draw_string(dc, vec2(320.0f, y), 1.5f, TEXT_CENTRE, colours::SILVER, "Xbox 360 Gamepad - **recommended**"); y += 15.0f;
	draw_string(dc, vec2(x0, y), 1.5f, TEXT_LEFT, colours::GREY, "Move");
	draw_string(dc, vec2(x1, y), 1.5f, TEXT_RIGHT, colours::GREY, "Left Stick"); y += 15.0f;
	draw_string(dc, vec2(x0, y), 1.5f, TEXT_LEFT, colours::GREY, "Shoot");
	draw_string(dc, vec2(x1, y), 1.5f, TEXT_RIGHT, colours::GREY, "Right Stick"); y += 15.0f;

	y += 25.0f;

	draw_string(dc, vec2(320.0f, y), 1.5f, TEXT_CENTRE, colours::SILVER, "Keyboard and Mouse"); y += 15.0f;
	draw_string(dc, vec2(x0, y), 1.5f, TEXT_LEFT, colours::GREY, "Move");
	draw_string(dc, vec2(x1, y), 1.5f, TEXT_RIGHT, colours::GREY, "%c%c%c%c", g_LocKeyW, g_LocKeyA, g_LocKeyS, g_LocKeyD); y += 15.0f;
	draw_string(dc, vec2(x0, y), 1.5f, TEXT_LEFT, colours::GREY, "Shoot");
	draw_string(dc, vec2(x1, y), 1.5f, TEXT_RIGHT, colours::GREY, "Mouse or \001\002\003\004"); y += 15.0f;

	draw_string(dc, vec2(320.0f, 330.0f), 2.0f, TEXT_CENTRE | TEXT_BOTTOM, colours::BLUE, "press space to play");
}