#include "Pch.h"
#include "Common.h"
#include "Game.h"

entity::entity(entity_type type) : _world(), _flags(), _type(type), _radius(10.0f) {
}

entity::~entity() {
}

void entity::destroy() {
	_flags |= EF_DESTROYED;
}

void entity::update() {
	if (_flags & EF_DESTROYED)
		return;

	tick();

	_old_pos = _pos;
	_pos += _vel * DT;

	// todo: random tetris shapes in arena?

	int hit = 0;

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

	if (_pos.x < a.x) { _pos.x = a.x; hit |= (int)side::XN; }
	if (_pos.x > b.x) { _pos.x = b.x; hit |= (int)side::XP; }
	if (_pos.y < a.y) { _pos.y = a.y; hit |= (int)side::YN; }
	if (_pos.y > b.y) { _pos.y = b.y; hit |= (int)side::YP; }

	if (hit)
		hit_wall(hit);

	post_tick();
}

void entity::render(draw_context* dc) {
	dc->rect(_pos - vec2(_radius), _pos + vec2(_radius), _colour);
}

void entity::tick() {
}

void entity::post_tick() {
}

void entity::hit_wall(int side) {
}