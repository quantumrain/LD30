#include "Pch.h"
#include "Common.h"
#include "Game.h"

tracker::tracker() : unit(ET_TRACKER), _drift_time(), _dizzy() {
	_flags |= EF_ENEMY;
	_colour = colours::BLUE * 1.25f;
	_radius = 8.0f;
}

void tracker::init() {
	SoundPlay(sound_id::TRACKER_SPAWN, _world->r.range(0.9f, 1.1f), _world->r.range(0.15f, 0.3f));
}

void tracker::tick() {
	if (_drift_time-- <= 0) {
		_drift_force	= _world->r.range(vec2(10.0f));
		_drift_time		= _world->r.range(30, 60);
	}

	if (_dizzy > 0) {
		_dizzy--;
		_vel *= 0.925f;

		_rot = normalise_radians(_rot + clamp(_dizzy / 50.0f, 0.0f, 0.25f));
	}
	else {
		_rot = normalise_radians(_rot * 0.9f);

		if (player* p = find_nearest_player(_world, _pos)) {
			_vel *= 0.9f;
			_vel += normalise(p->_pos - _pos) * 20.0f;
			_vel += _drift_force;
		}
	}

	avoid_crowd(_world, this, true);
}

void tracker::post_tick() {
	unit::post_tick();
}

void tracker::hit_wall(int clipped) {
	if (clipped & CLIPPED_XN) _vel.x = fabsf(_vel.x);
	if (clipped & CLIPPED_XP) _vel.x = -fabsf(_vel.x);
	if (clipped & CLIPPED_YN) _vel.y = fabsf(_vel.y);
	if (clipped & CLIPPED_YP) _vel.y = -fabsf(_vel.y);
}

void tracker::draw(draw_context* dc) {
	for(int i = 0; i < 2; i++) {
		float f = (i / 2.0f) * PI * 0.5f;
		dc->push_transform(rotate_z(f));
		dc->rect(-vec2(_radius), vec2(_radius), _colour);
		dc->pop_transform();
	}

	float r = _radius * 0.75f;
	colour c = colour(0.0f, 0.0f, 0.0f, 1.0f);

	for(int i = 0; i < 2; i++) {
		float f = (i / 2.0f) * PI * 0.5f;
		dc->push_transform(rotate_z(f));
		dc->rect(-vec2(r), vec2(r), c);
		dc->pop_transform();
	}
}

void tracker::damage(damage_desc* dd) {
	if (dd->type == damage_type::PUSH) {
		if (dd->cause) {
			_vel += dd->cause->_vel * 0.05f;
			_dizzy = 25 + (_dizzy % 25);
		}
	}
	else
		unit::damage(dd);
}
