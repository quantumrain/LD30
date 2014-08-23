#include "Pch.h"
#include "Common.h"
#include "Game.h"

shooting_star::shooting_star() : entity(ET_SHOOTING_STAR) {
	_flags |= EF_USE_OUTER_LIMIT | EF_ENEMY;
	_colour = colours::AQUA;
	_radius = 6.0f;
}

void shooting_star::tick() {
}

void shooting_star::post_tick() {
	//if (
}

void shooting_star::hit_wall(int side) {
	destroy();
}

void spawn_shooting_star(world* w, entity* instigator) {
	if (w->r.chance(4, 5))
		return;

	shooting_star* ent = spawn_entity(w, new shooting_star);

	ent->_pos = instigator->_pos;
	ent->_vel = normalise(w->player->_pos - ent->_pos) * 250.0f + w->r.range(vec2(10.0f));
}