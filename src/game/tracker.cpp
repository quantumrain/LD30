#include "Pch.h"
#include "Common.h"
#include "Game.h"

tracker::tracker() : unit(ET_TRACKER) {
	_flags |= EF_ENEMY;
	_colour = colours::PURPLE;
	_radius = 8.0f;
}

void tracker::tick() {
	if (player* p = find_nearest_player(_world, _pos)) {
		_vel *= 0.9f;
		_vel += normalise(p->_pos - _pos) * 24.0f;
	}

	avoid_crowd(_world, this);
}

void tracker::post_tick() {
	if (overlaps_player(this)) {
		//SoundPlay(sound_id::DIT, 1.0f, 1.0f);
		destroy();
	}
}

void tracker::hit_wall(int clipped) {
	if (clipped & CLIPPED_XN) _vel.x = fabsf(_vel.x);
	if (clipped & CLIPPED_XP) _vel.x = -fabsf(_vel.x);
	if (clipped & CLIPPED_YN) _vel.y = fabsf(_vel.y);
	if (clipped & CLIPPED_YP) _vel.y = -fabsf(_vel.y);
}
