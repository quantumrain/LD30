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

	if (unit* u = find_overlapping_unit(_world, _pos, ET_ASTEROID, _radius)) {
		u->damage(&damage_desc(damage_type::PUSH, 0, this));
		spawn_pickup(_world, _pos);
		destroy();
	}
}

void shooting_star::hit_wall(int clipped) {
	destroy();
}

void shooting_star::damage(damage_desc* dd) {
	if (dd->type != damage_type::BULLET)
		unit::damage(dd);
}

void spawn_shooting_star(world* w, entity* instigator) {
	if (!w->player)
		return;

	if (w->r.chance(3, 5))
		return;

	shooting_star* ent = spawn_entity(w, new shooting_star);

	vec2 p = instigator->_pos;
	vec2 v = normalise(w->player->_pos - p) * 250.0f + w->r.range(vec2(10.0f));

	ent->_pos = p;
	ent->_vel = v;
}