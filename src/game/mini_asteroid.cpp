#include "Pch.h"
#include "Common.h"
#include "Game.h"

mini_asteroid::mini_asteroid() : unit(ET_MINI_ASTEROID) {
	_flags |= EF_USE_OUTER_LIMIT | EF_ENEMY;
	_colour = colours::RED;
	_radius = 6.0f;

	instant_spawn();
}

void mini_asteroid::tick() {
}

void mini_asteroid::post_tick() {
	unit::post_tick();
}

void mini_asteroid::hit_wall(int clipped) {
	destroy();
}

void mini_asteroid::damage(damage_desc* dd) {
	if (dd->type != damage_type::BULLET)
		unit::damage(dd);
}

void spawn_mini_asteroids(world* w, vec2 pos) {
	for(int i = 0; i < 3; i++) {
		mini_asteroid* ent = spawn_entity(w, new mini_asteroid);

		ent->_pos = pos;
		ent->_vel = rotation(w->r.range(PI)) * w->r.range(100.0f, 150.0f);
	}
}