#include "Pch.h"
#include "Common.h"
#include "Game.h"

extern vec2 g_mouse_screen;

player::player() : unit(ET_PLAYER), _reload_time(), _health_recharge(), _shield_time() {
	_flags |= EF_PLAYER;
	_colour = colours::ORANGE * 1.5f;
	_radius = 6.0f;
	_health = 3;
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

	if (_health < 3) {
		if (++_health_recharge >= (5 * 60)) {
			_health_recharge = 0;
			_health = clamp(_health + 1, 0, 3);

			SoundPlay(sound_id::DIT, 3.0f, 1.0f);
		}
	}

	if (_shield_time > 0)
		_shield_time--;
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
	colour c;

	switch(_health) {
		default:
		case 1: c = colours::BLUE * 1.5f; break;
		case 2: c = colours::GREEN * 1.5f; break;
		case 3: c = colours::ORANGE * 1.5f; break;
	}

	if (_shield_time > 0) {
		dc->rect(-vec2(_radius * 1.5f), vec2(_radius * 1.5f), c);
		dc->rect(-vec2(_radius * 1.25f), vec2(_radius * 1.25f), colour(0.0f, 1.0f));
	}

	dc->rect(-vec2(_radius), vec2(_radius), c);
	dc->rect(-vec2(_radius * 0.75f), vec2(_radius * 0.75f), colour(0.0f, 1.0f));

	for(int i = 0; i < _health; i++) {
		float w = _radius * 0.5f;
		float h = (_radius * 0.5f) / 4.0f;

		float y = h * (1 - i) * 3.0f;

		dc->rect(vec2(-w, y - h), vec2(w, y + h), c);
	}
}

void player::damage(damage_desc* dd) {
	if (_shield_time > 0)
		return;

	unit::damage(dd);
}

void player::flinch(damage_desc* dd) {
	_health_recharge = 0;
	_shield_time = 30;

	SoundPlay(sound_id::DIT, 1.0f, 1.0f);
}

void player::killed(damage_desc* dd) {
	SoundPlay(sound_id::DIT, 0.1f, 1.0f);
	destroy();
}

void player::try_fire(vec2 dir) {
	if (_reload_time > 0)
		return;

	_reload_time = 4;

	vec2 p = _pos - _vel * DT;
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
	}
}