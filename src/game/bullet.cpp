#include "Pch.h"
#include "Common.h"
#include "Game.h"

bullet::bullet() : entity(ET_BULLET) {
	_flags |= EF_BULLET;
	_colour = colours::ORANGE;
	_radius = 4.0f;
	_time = 60;
}

void bullet::tick() {
	if (--_time <= 0)
		destroy();
}

void bullet::post_tick() {
}

void bullet::hit_wall(int side) {
	_time = 0;
	spawn_shooting_star(_world, this);
}