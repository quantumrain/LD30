#include "Pch.h"
#include "Common.h"
#include "Game.h"

entity* spawn_entity(world* w, entity* ent) {
	if (ent) {
		ent->_world = w;
		w->entities.push_back(ent);
	}

	return ent;
}

void world_init(world* w) {
	w->limit		= { { -320.0f, -180.0f }, { 320.0f, 180.0f } };
	w->outer_limit	= inflate(w->limit, 80.0f);

	w->player = spawn_entity(w, new player);
}

void world_update(world* w) {
	for(int i = 0; i < w->entities.size(); i++) {
		w->entities[i]->update();
	}

	for(int i = 0; i < w->entities.size(); i++) {
		entity* e = w->entities[i];

		if (e->_flags & EF_DESTROYED) {
			w->entities.swap_remove(i);
			i--;
		}
	}
}

void world_render(world* w, draw_context* dc) {
	for(auto e : w->entities) if (e->_flags & EF_BULLET) e->render(dc);
	for(auto e : w->entities) if (e->_flags & EF_ENEMY) e->render(dc);

	if (w->player)
		w->player->render(dc);

	vec2 p0 = w->limit.min;
	vec2 p3 = w->limit.max;
	vec2 p1 = { p3.x, p0.y };
	vec2 p2 = { p0.x, p3.y };

	colour c = colours::AQUA * 0.5f;

	dc->line(p0, p1, 1.0f, c);
	dc->line(p1, p3, 1.0f, c);
	dc->line(p3, p2, 1.0f, c);
	dc->line(p2, p0, 1.0f, c);
}