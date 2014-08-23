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

mat44 make_proj_view(vec2 centre, float fov, float virtual_height) {
	float camera_height = virtual_height / (tan(deg_to_rad(fov) * 0.5f) * 2.0f);

	mat44 proj = perspective(deg_to_rad(fov), g_view_aspect, camera_height * 0.1f, camera_height * 2.0f);
	mat44 view = camera_look_at(vec3(centre, camera_height), vec3(centre, 0.0f), vec3(0, -1, 0));

	return proj * view;
}

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

	mat44 proj_view	= make_proj_view(vec2(), 90.0f, 360.0f + 20.0f);
	frustum fr		= make_frustum(proj_view);
	vec4 draw_plane	= { 0, 0, 1, 0 };
	aabb2 field		= { (vec2)intersect_planes_3(fr.left(), fr.top(), draw_plane), (vec2)intersect_planes_3(fr.right(), fr.bottom(), draw_plane) };

	set_proj_view(proj_view);

	world_render(&g_world, &dc);
}