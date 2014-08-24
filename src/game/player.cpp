#include "Pch.h"
#include "Common.h"
#include "Game.h"

extern vec2 g_mouse_screen;

player::player() : unit(ET_PLAYER), _reload_time(), _health_recharge(), _shield_time(), _recharge_pulse(), _rot_v(), _tada() {
	_flags |= EF_PLAYER;
	_colour = colours::GREEN * 1.5f;
	_radius = 6.0f;
	_health = 3;
	_shield_time = 40;
}

void player::init() {
	SoundPlay(sound_id::PLAYER_SPAWN, 1.0f, 1.5f);
}

void player::tick() {
	vec2 acc = gJoyLeft;

	if (is_key_down(KEY_L_LEFT)) acc.x -= 1.0f;
	if (is_key_down(KEY_L_RIGHT)) acc.x += 1.0f;
	if (is_key_down(KEY_L_UP)) acc.y -= 1.0f;
	if (is_key_down(KEY_L_DOWN)) acc.y += 1.0f;

	if (length_sq(acc) > 1.0f)
		acc = normalise(acc);

	_vel *= 0.8f;
	_vel += acc * 55.0f;

	if (length_sq(_vel) > 0.1f) {
		_rot_v += dot(perp(normalise(_vel)), acc) * 4.0f;
	}

	_rot_v += -_rot;
	_rot_v *= 0.9f;

	_rot = normalise_radians(_rot + (_rot_v * DT));

	if (_health < 3) {
		if (++_health_recharge >= (5 * 60)) {
			_health_recharge = 0;
			_recharge_pulse = 10;
			_health = clamp(_health + 1, 0, 3);

			SoundPlay(sound_id::PLAYER_RECHARGE, 1.0f, 1.0f);
		}
	}

	if (_shield_time > 0) {
		if (--_shield_time <= 0) {
			SoundPlay(sound_id::PLAYER_SHIELD_END, 1.0f, 0.75f);
		}
	}

	if (_recharge_pulse > 0)
		_recharge_pulse--;

	if (!_tada) {
		//SoundPlay(sound_id::PLAYER_SPAWN_END, 1.0f, 1.0f);
		_tada = true;
	}
}

void player::post_tick() {
	vec2 dir = gJoyRight;

	if (is_key_down(KEY_R_LEFT)) dir.x -= 1.0f;
	if (is_key_down(KEY_R_RIGHT)) dir.x += 1.0f;
	if (is_key_down(KEY_R_UP)) dir.y -= 1.0f;
	if (is_key_down(KEY_R_DOWN)) dir.y += 1.0f;

	if (gMouseButtons & 1) dir += g_mouse_screen - _pos;

	if (_reload_time > 0)
		_reload_time--;

	if (length_sq(dir) > 0.2f) {
		try_fire(normalise(dir));
	}
}

void player::draw(draw_context* dc) {
	float r = _radius + square(_recharge_pulse / 10.0f) * 4.0f;

	colour c;

	switch(_health) {
		default:
		case 1: c = colour(1.0f, 0.25f, 0.5f, 1.0f); break;
		case 2: c = colours::ORANGE; break;
		case 3: c = colours::GREEN; break;
	}

	c *= colour(1.5f, 1.0f);

	if (_shield_time > 0) {
		dc->rect(-vec2(r * 1.5f), vec2(r * 1.5f), c * 2.5f);
		dc->rect(-vec2(r * 1.25f), vec2(r * 1.25f), colour(0.0f, 1.0f));
	}

	dc->rect(-vec2(r), vec2(r), c);
	dc->rect(-vec2(r * 0.75f), vec2(r * 0.75f), colour(0.0f, 1.0f));

	float oy = (_health - 1) * 0.5f;

	for(int i = 0; i < _health; i++) {
		float w = r * 0.5f;
		float h = (r * 0.5f) / 4.0f;

		float y = h * (i - oy) * 3.0f;

		dc->rect(vec2(-w, y - h), vec2(w, y + h), c);// lerp(c, colour(), 0.5f));
	}
}

void player::damage(damage_desc* dd) {
	if (_shield_time > 0)
		return;

	unit::damage(dd);
}

void player::flinch(damage_desc* dd) {
	_health_recharge = 0;
	_shield_time = 60;
	_recharge_pulse = 10;

	SoundPlay(sound_id::PLAYER_HIT, 1.0f, 1.0f);

	spawn_bomb(_world, _pos);
}

void player::killed(damage_desc* dd) {
	fx_explosion(_pos, 3.0f, 100, colours::ORANGE * 1.5f, 1.0f);

	SoundPlay(sound_id::PLAYER_DIE, 1.0f, 1.0f);

	_world->shake += 50.0f;

	destroy();
}

void player::try_fire(vec2 dir) {
	if (_reload_time > 0)
		return;

	SoundPlay(sound_id::PLAYER_FIRE, _world->r.range(1.0f, 1.0f), lerp(0.1f, 0.85f, square(_world->r.rand(1.0f))));

	_reload_time = 4;

	vec2 p = _pos;// - _vel * DT;
	vec2 base_v = _vel;

	if (gMouseButtons & 1) base_v -= perp(dir) * dot(base_v, perp(dir)) * 0.75f; // todo: update if cam tracking changes

	for(int i = -1; i <= 1; i++) {
		bullet* b = spawn_entity(_world, new bullet);

		vec2 d = dir * 800.0f;
		
		if (i != 0)
			d = (d * 0.9f) + (perp(dir) * (15.0f * i));

		b->_pos		= p;
		b->_old_pos	= p;
		b->_vel		= base_v + d;
		b->_rot		= rotation_of(d);
	}

	for(int i = 0; i < 8; i++) {
		psys_spawn(_pos + dir * _radius * 1.85f, vec2(), 0.0f, 7.5f, 7.5f, 0.0f, colour(0.75f, 0.0f), 2);
	}
}