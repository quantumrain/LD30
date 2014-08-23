#include "Pch.h"
#include "Common.h"
#include "Game.h"

tracker::tracker() : entity(ET_TRACKER) {
	_flags |= EF_ENEMY;
	_colour = colours::PURPLE;
	_radius = 8.0f;
}

void tracker::tick() {
	if (player* p = find_nearest_player(_world, _pos)) {
		_vel *= 0.8f;
		_vel += normalise(p->_pos - _pos) * 40.0f;
	}
}

void tracker::post_tick() {
	if (overlaps_player(this)) {
		SoundPlay(sound_id::DIT, 1.0f, 1.0f);
		destroy();
	}
}

void tracker::hit_wall(int side) {
	if (side & (int)side::XN) _vel.x = fabsf(_vel.x);
	if (side & (int)side::XP) _vel.x = -fabsf(_vel.x);
	if (side & (int)side::YN) _vel.x = fabsf(_vel.y);
	if (side & (int)side::YP) _vel.x = -fabsf(_vel.y);
}
