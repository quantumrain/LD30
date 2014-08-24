#include "Pch.h"
#include "Common.h"
#include "Game.h"

extern random g_rand;

asteroid::asteroid() : unit(ET_ASTEROID), _rot_v() {
	_flags |= EF_USE_OUTER_LIMIT | EF_ENEMY;
	_colour = colours::RED;
	_radius = 16.0f;

	instant_spawn();
}

void asteroid::init() {
	for(int i = 0; i < NUM_AST_PROFILE; i++) {
		float r = _world->r.range(0.95f, 1.3f);

		if (_world->r.chance(1, 5))
			r *= 0.85f;

		_profile[i] = r;
	}

	_rot_v = _world->r.range(6.0f);
}

void asteroid::tick() {
	_rot += _rot_v * DT;
	avoid_crowd(_world, this, false);
}

void asteroid::post_tick() {
	unit::post_tick();

	for(int i = 0; i < 2; i++) {
		vec2 p = _pos + rotation(g_rand.range(PI)) * square(g_rand.rand(1.0f)) * _radius * 1.25f;

		psys_spawn(p, 0.0f, 0.9f, 2.0f, 2.0f,  g_rand.range(1.0f), _colour * 0.5f, g_rand.range(24, 40));
	}
}

void asteroid::hit_wall(int clipped) {
	destroy();
}

void asteroid::draw(draw_context* dc) {
	vec2 p[NUM_AST_PROFILE];

	for(int i = 0; i < NUM_AST_PROFILE; i++) {
		float f = (i / (float)NUM_AST_PROFILE) * PI_2;

		p[i] = rotation(f) * _profile[i];
	}

	float thi = 1.5f;

	for(int i = 0, j = NUM_AST_PROFILE - 1; i < NUM_AST_PROFILE; j = i, i++) dc->tri(vec2(), p[i] * _radius, p[j] * _radius, _colour);
	for(int i = 0, j = NUM_AST_PROFILE - 1; i < NUM_AST_PROFILE; j = i, i++) dc->tri(vec2(), p[i] * (_radius - thi), p[j] * (_radius - thi), colour(0.0f, 1.0f));
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
	else if (dd->type == damage_type::BOMB) {
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