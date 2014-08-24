#include "Pch.h"
#include "Common.h"
#include "Game.h"

pickup::pickup() : entity(ET_PICKUP) {
	_flags |= EF_PICKUP;
	_colour = colour(0.5f, 1.0f, 0.1f, 1.0f) * 1.5f;
	_radius = 1.75f;
	_time = 60 * 3;
}

void pickup::init() {
	_vel = _world->r.range(vec2(30.0f));
}

void pickup::tick() {
	if (--_time <= 0)
		destroy();

	if (player* p = find_nearest_player(_world, _pos)) {
		vec2 d = p->_pos - _pos;
		float l = length_sq(d);

		if (l < square(_radius + p->_radius)) {
			float r[3] = { 1.0f, 1.2f, 1.5f };
			static int roll;

			if (SoundPlay(sound_id::PICKUP_COLLECT, r[roll], _world->r.range(0.4f, 0.6f)))
				roll = (roll + 1) % 3;
			
			_world->score += 5 * _world->multi;

			if (_world->score >= _world->warning_at) {
				spawn_message(_world, _pos, true, "%I64i", _world->warning_at);

				if (_world->warning_at < 10000)				_world->warning_at += 1000;
				else if (_world->warning_at < 100000)		_world->warning_at += 10000;
				else if (_world->warning_at < 1000000)		_world->warning_at += 100000;
				else if (_world->warning_at < 10000000)		_world->warning_at += 1000000;
				else if (_world->warning_at < 100000000)	_world->warning_at += 10000000;
			}

			destroy();
		}
		else if (l < square(48.0f)) {
			_vel += d * (60.0f / sqrt(l));
			_time = max(_time, 60);
		}
	}

	if (length_sq(_vel) > 30.0f)
		_vel *= 0.9f;

	// avoid asteroids

	for(auto e : _world->entities) {
		if (e->_flags & EF_DESTROYED)
			continue;

		if (e->_type != ET_ASTEROID)
			continue;

		vec2	d		= _pos - e->_pos;
		float	min_l	= _radius + e->_radius;

		if (length_sq(d) > square(min_l))
			continue;

		float l = length(d);

		if (l > 0.1f) {
			_vel += d * (8.0f / l);
		}
	}
}

void pickup::post_tick() {
}

void pickup::hit_wall(int clipped) {
	if (clipped & CLIPPED_XN) _vel.x = fabsf(_vel.x);
	if (clipped & CLIPPED_XP) _vel.x = -fabsf(_vel.x);
	if (clipped & CLIPPED_YN) _vel.y = fabsf(_vel.y);
	if (clipped & CLIPPED_YP) _vel.y = -fabsf(_vel.y);
}

void pickup::draw(draw_context* dc) {
	if ((_time < 60) && ((_time & 3) < 2))
		return;

	dc->rect(-vec2(_radius), vec2(_radius), _colour);

	dc->push_transform(rotate_z(deg_to_rad(45.0f)));
		dc->rect(-vec2(_radius), vec2(_radius), _colour);
	dc->pop_transform();

	if (_world->level_time < 400) {
		float f = clamp(400 - _world->level_time, 0, 15) / 15.0f;

		if (_world->score < 15) {
			if (!(_flags & EF_SPAWNING))
				draw_string(dc, vec2(_radius + 4.0f, 0.0f), 0.5f, TEXT_VCENTRE, colour(f), "COLLECT ME!");
		}
	}
}

void spawn_pickup(world* w, vec2 pos) {
	pickup* ent = spawn_entity(w, new pickup);

	ent->_pos = pos;
}