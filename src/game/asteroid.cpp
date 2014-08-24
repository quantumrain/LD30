#include "Pch.h"
#include "Common.h"
#include "Game.h"

asteroid::asteroid() : unit(ET_ASTEROID) {
	_flags |= EF_USE_OUTER_LIMIT | EF_ENEMY;
	_colour = colours::RED;
	_radius = 12.0f;

	instant_spawn();
}

void asteroid::tick() {
	avoid_crowd(_world, this, false);
}

void asteroid::post_tick() {
	unit::post_tick();
}

void asteroid::hit_wall(int clipped) {
	destroy();
}

void asteroid::damage(damage_desc* dd) {
	if (dd->type == damage_type::BULLET) {
		if (dd->cause)
			_vel += dd->cause->_vel * 0.0015f;
	}
	else if (dd->type == damage_type::PUSH) {
		if (dd->cause)
			_vel += dd->cause->_vel * 0.025f;
	}
	else if (dd->type == damage_type::COLLISION) {
		if (dd->cause) {
			vec2 d = normalise(_pos - dd->cause->_pos);

			dd->cause->_vel -= d * 200.0f;
			_vel += d * 20.0f;
		}
	}
}

vec2 asteroid_target(world* w, int f, float radius) {
	switch(f) {
		default:
		case 0: return { w->r.range(w->limit.min.x, w->limit.max.x),		w->outer_limit.min.y + radius };
		case 1: return { w->r.range(w->limit.min.x, w->limit.max.x),		w->outer_limit.max.y - radius };
		case 2: return { w->outer_limit.min.x + radius,						w->r.range(w->limit.min.y, w->limit.max.y) };
		case 3: return { w->outer_limit.max.x - radius,						w->r.range(w->limit.min.y, w->limit.max.y) };
	}
}

void spawn_asteroid(world* w) {
	asteroid* ent = spawn_entity(w, new asteroid);

	int f = w->r.rand(4);

	vec2 p = asteroid_target(w, f, ent->_radius + 1.0f);
	vec2 q = asteroid_target(w, f ^ 1, ent->_radius + 1.0f);

	vec2 v = normalise(q - p) * w->r.range(50.0f, 100.0f);

	ent->_pos = p;
	ent->_vel = v;
}