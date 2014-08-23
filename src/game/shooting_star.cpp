#include "Pch.h"
#include "Common.h"
#include "Game.h"

shooting_star::shooting_star() : unit(ET_SHOOTING_STAR) {
	_flags |= EF_USE_OUTER_LIMIT | EF_ENEMY;
	_colour = colours::AQUA;
	_radius = 6.0f;

//	instant_spawn();
}

void shooting_star::tick() {
}

void shooting_star::post_tick() {
	unit::post_tick();
}

void shooting_star::hit_wall(int clipped) {
	destroy();
}

void spawn_shooting_star(world* w, entity* instigator) {
	if (w->r.chance(3, 5))
		return;

	shooting_star* ent = spawn_entity(w, new shooting_star);

	vec2 p = { w->r.range(w->limit.min.x, w->limit.max.x), lerp(w->limit.min.y, w->outer_limit.min.y, 0.5f) };
	vec2 v = { 0.0f, w->r.range(100.0f, 150.0f) };

	//vec2 p = instigator->_pos;
	//vec2 v = normalise(w->player->_pos - ent->_pos) * 250.0f + w->r.range(vec2(10.0f));

	ent->_pos = p;
	ent->_vel = v;
}