#include "Pch.h"
#include "Common.h"
#include "Game.h"

extern random g_rand;

shooting_star::shooting_star() : unit(ET_SHOOTING_STAR), _flash() {
	_flags |= EF_USE_OUTER_LIMIT | EF_ENEMY;
	_colour = colour(1.0f, 0.15f, 0.1f, 1.0f);
	_radius = 5.0f;

//	instant_spawn();
}

void shooting_star::tick() {
	if (_flash > 0)
		_flash--;

	_vel = lerp(_vel, _desired_vel, 0.1f);
}

void shooting_star::post_tick() {
	unit::post_tick();

	if (unit* u = find_overlapping_unit(_world, _pos, ET_ASTEROID, _radius)) {
		fx_explosion(_pos, 0.5f, 5, _colour, 1.0f);

		SoundPlay(sound_id::SHOOTING_STAR_HIT, _world->r.range(0.9f, 1.1), _world->r.range(0.6f, 0.8));

		if (_world->shake < 3.0f) _world->shake += 0.5f;

		u->damage(&damage_desc(damage_type::PUSH, 0, this));
		spawn_pickup(_world, _pos);
		destroy();
	}

	if (unit* u = find_overlapping_unit(_world, _pos, ET_TRACKER, _radius)) {
		fx_explosion(_pos, 0.5f, 5, _colour, 1.0f);

		SoundPlay(sound_id::SHOOTING_STAR_HIT, _world->r.range(0.9f, 1.1), _world->r.range(0.6f, 0.8));

		if (_world->shake < 3.0f) _world->shake += 0.5f;

		u->damage(&damage_desc(damage_type::PUSH, 0, this));
		spawn_pickup(_world, _pos);
		destroy();
	}

	_rot = rotation_of(_vel);

	psys_spawn(_pos, 0.0f, 0.9f, 3.0f, 1.0f, g_rand.range(1.0f), _colour * 0.3f, g_rand.range(24, 40));
}

void shooting_star::hit_wall(int clipped) {
	destroy();
}

void shooting_star::draw(draw_context* dc) {
	float fc = _flash / 2.0f;

	colour cc = lerp(_colour, colour(10.0f), fc);
	colour c = lerp(colour(0.0f, 1.0f), colour(10.0f, 10.0f, 10.0f, 0.85f), fc);

	for(int i = 0; i < 4; i++) {
		float f = (i / 4.0f) * PI * 0.5f;
		dc->push_transform(rotate_z(f));
		dc->rect(-vec2(_radius), vec2(_radius), cc);
		dc->pop_transform();
	}

	float r = _radius * 0.75f;

	for(int i = 0; i < 4; i++) {
		float f = (i / 4.0f) * PI * 0.5f;
		dc->push_transform(rotate_z(f));
		dc->rect(-vec2(r), vec2(r), c);
		dc->pop_transform();
	}
}

void shooting_star::damage(damage_desc* dd) {
	if (dd->type != damage_type::BULLET) {
		unit::damage(dd);
	}
	else {
		for(int i = 0; i < 4; i++) {
			psys_spawn(dd->cause->_pos, vec2(), 0.0f, 10.0f, 10.0f, 0.0f, colour(), 2);
		}

		_flash = 2;
	}
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
	ent->_desired_vel = v;
	ent->_rot = rotation_of(v);
}