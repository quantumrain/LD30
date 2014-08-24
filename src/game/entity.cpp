#include "Pch.h"
#include "Common.h"
#include "Game.h"

#define SPAWN_TIME 30

entity::entity(entity_type type) : _world(), _flags(), _type(type), _radius(10.0f), _spawn_timer() {
}

entity::~entity() {
}

void entity::destroy() {
	_flags |= EF_DESTROYED;
}

void entity::update() {
	if (_flags & EF_DESTROYED)
		return;

	if (_flags & EF_SPAWNING) {
		if (++_spawn_timer >= SPAWN_TIME)
			_flags &= ~EF_SPAWNING;
	}

	if (!(_flags & EF_SPAWNING)) {
		tick();

		_old_pos = _pos;
		_pos += _vel * DT;

		// todo: random tetris shapes in arena?

		int clipped = 0;

		vec2 a;
		vec2 b;

		if (_flags & EF_USE_OUTER_LIMIT) {
			a = _world->outer_limit.min + _radius;
			b = _world->outer_limit.max - _radius;
		}
		else {
			a = _world->limit.min + _radius;
			b = _world->limit.max - _radius;
		}

		// todo: should clip by the line rather than clamping back

		if (_pos.x < a.x) { _pos.x = a.x; clipped |= CLIPPED_XN; }
		if (_pos.x > b.x) { _pos.x = b.x; clipped |= CLIPPED_XP; }
		if (_pos.y < a.y) { _pos.y = a.y; clipped |= CLIPPED_YN; }
		if (_pos.y > b.y) { _pos.y = b.y; clipped |= CLIPPED_YP; }

		if (clipped)
			hit_wall(clipped);

		post_tick();
	}
}

void entity::render(draw_context* dc) {
	dc->push_transform(translate(vec3(_pos, 0.0f)));

	if (_flags & EF_SPAWNING) {
		float t = _spawn_timer / (float)SPAWN_TIME;

		for(int i = 0; i < 3; i++) {
			float a = (t * 2.0f) - (i / 3.0f);

			if (a < 0.0f)
				continue;

			float f = fmodf(a, 1.0f);
			float s = f * 3.0f;
			float c = clamp(min(3.0f - s, s), 0.0f, 1.0f);

			dc->push_transform(scale(s));
			dc->push_colour(colour(c * 0.5f, 0.0f));
			draw(dc);
			dc->pop_colour();
			dc->pop_transform();
		}
	}
	else
		draw(dc);

	dc->pop_transform();
}

void entity::init() {
}

void entity::tick() {
}

void entity::post_tick() {
}

void entity::hit_wall(int clipped) {
}

void entity::draw(draw_context* dc) {
	dc->rect(-vec2(_radius), vec2(_radius), _colour);
}