#include "Pch.h"
#include "Common.h"
#include "Game.h"

/*

	TODO:

	dodging enemies?

	decide whether to keep multiplier, or just use collection for score (crazy idea - kills = mulitplier, pickup = score?)

	display score + multiplier as popup text in arena on collect/kill (say, every 1000 for first 10k, then every 2500, etc...)

*/

enum class game_state {
	MENU,
	GAME
};

const wchar_t* gAppName = L"LD30 - Connected Worlds";

game_state g_game_state;
world g_world;

void game_init() {
}

void game_update() {
	world* w = &g_world;

	switch(g_game_state) {
		case game_state::MENU:
			if (menu_update()) {
				world_init(w);
				g_game_state = game_state::GAME;
			}
		break;

		case game_state::GAME: {
			bool reset = is_key_down(KEY_ESCAPE);

			if (g_world.player == 0) {
				if (is_key_down(KEY_FIRE) || (gMouseButtons & 1) || gJoyA)
					reset = true;
			}

			if (reset) {
				world_clear(w);
				g_game_state = game_state::MENU;
			}
			else {
				world_update(w);
			}
		}
		break;
	}
}

void game_render() {
	draw_context dc(&g_dl_world);
	draw_context dc_ui(&g_dl_ui);

	dc.set_texture(g_sheet);

	switch(g_game_state) {
		case game_state::MENU:
			menu_render(&dc_ui);
		break;

		case game_state::GAME:
			world_render(&g_world, &dc);

			draw_string(&dc_ui, vec2(5.0f, 5.0f), 1.5f, 0, colours::SILVER, "%I64i", g_world.score);
			draw_string(&dc_ui, vec2(5.0f, 17.5f), 0.75f, 0, colours::SILVER, "x%I64i", g_world.multi);
		break;
	}
}