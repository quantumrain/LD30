#include "Pch.h"
#include "Common.h"
#include "Game.h"

/*

	Everything that happens in the game is connected?

	Miss a shot and once your shots hit the wall they turn into fast moving projectiles from the top/left-ish-random of the screen, indestructable must be dodged (shooting star - light blue)

	Kill an enemy, spawn a slower moving but larger projectile from the top/right of screen (asteroid - red)

	If any of these spawned projectiles hit each other they'll bounce off each other making crazy trajectories
	If a shooting star hits an asteroid, the asteroid will split into... 3 (?) randomly aimed mini-asteroids, further hits just null-ify

	Standard killable enemies either track, or track and dodge...?


*/

const wchar_t* gAppName = L"LD30 - Connected Worlds";

world g_world;

void game_init() {
	world_init(&g_world);
}

void game_update() {
	world_update(&g_world);
}

void game_render() {
	draw_context dc(&g_dl_world);
	draw_context dc_ui(&g_dl_ui);

	dc.set_texture(g_sheet);

	world_render(&g_world, &dc);
}