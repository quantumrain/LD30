#include "Pch.h"
#include "Common.h"
#include "Game.h"

bullet::bullet() : entity(ET_BULLET) {
	_flags |= EF_BULLET;
	_colour = colour(colours::ORANGE, 0.0f);
	_radius = 4.0f;
	_time = 60;
}

void bullet::tick() {
	if (--_time <= 0)
		destroy();
}

void bullet::post_tick() {
	if (unit* u = find_enemy_near_line(_world, _old_pos, _pos, _radius)) {
		fx_explosion(_pos, 0.3f, 1, _colour);

		u->damage(&damage_desc(damage_type::BULLET, 1, this));
		destroy();
	}

	_rot = rotation_of(_vel);
}

void bullet::hit_wall(int clipped) {
	_time = 0;
	spawn_shooting_star(_world, this);

	fx_explosion(_pos, 0.3f, 1, _colour);
}

void bullet::render(draw_context* dc) {
	for(int i = 0; i <= 4; i++) {
		float f = i / 4.0f;

		vec2	p = lerp(_old_pos, _pos, f);
		colour	c = lerp(_colour, colour(0.0f), 1.0f - f);

		dc->push_transform(translate(vec3(p, 0.0f)) * rotate_z(_rot));
		dc->rect(-vec2(_radius), vec2(_radius), c);
		dc->pop_transform();
	}
}