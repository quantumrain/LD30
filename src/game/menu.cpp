#include "Pch.h"
#include "Common.h"
#include "Game.h"

bool menu_update() {
	return is_key_down(KEY_FIRE) || (gMouseButtons & 1) || gJoyA;
}

void menu_render(draw_context* dc) {
	draw_string(dc, vec2(320.0f, 180.0f), 1.0f, 0, colours::SILVER, "press fire");
}