#include "Pch.h"
#include "Common.h"
#include "Game.h"

vec2 g_mouse_screen;

template<typename C, typename F> void for_all(C& c, F&& f) {
	for(int i = 0; i < c.size(); i++) {
		if (!(c[i]->_flags & EF_DESTROYED)) f(c[i]);
	}
}

template<typename C> void prune(C& c) {
	for(int i = 0; i < c.size(); ) {
		if (c[i]->_flags & EF_DESTROYED) c.swap_remove(i); else i++;
	}
}

// world

world::world() : player(), spawn_time(), level_time(), kills(), score(), multi(), hiscore() {
}

void world_init(world* w) {
	w->r = random(GetTickCount());

	w->limit		= { { -320.0f, -180.0f }, { 320.0f, 180.0f } };
	w->outer_limit	= { w->limit.min * 1.5f, w->limit.max * 1.5f };

	spawn_entity(w, new player);

	w->spawn_time	= 30;
	w->level_time	= 0;
	w->kills		= 0;

	w->score		= 0;
	w->multi		= 1;
}

void world_clear(world* w) {
	w->entities.free();
	w->player = 0;
}

void world_update(world* w) {
	g_mouse_screen = (vec2)unproject(vec3(to_vec2(gMousePos), 0.0f), inverse(w->proj_view), to_vec2(g_view_size));

	// update

	// todo: players and bullets first, then enemies? possibly post spawn too?

	for_all(w->entities, [](entity* e) { e->update(); });

	// camera

	if (w->player) {
		w->camera_target = lerp(w->camera_target, w->player->_pos * 0.25f, 0.2f);
	}

	// spawn

	if (player* pl = w->player) {
		int num_trackers = 0;

		for_all(w->entities, [&](entity* e) { if (e->_type == ET_TRACKER) num_trackers++; });

		w->level_time++;

		float	f = 50.0f / (50.0f + w->kills);
		int		t = (int)(90 * f);

		if (++w->spawn_time > t) {
			tracker* t = spawn_entity(w, new tracker);

			vec2 p;

			for(int i = 0; i < 10; i++) {
				p = w->r.range(w->limit.min + t->_radius, w->limit.max - t->_radius);

				if (length_sq(pl->_pos - p) > square(100.0f))
					break;
			}

			t->_pos = p;
			t->_old_pos = p;

			w->spawn_time = 0;
		}

		if ((w->level_time % 120) == 0) {
			int num_spawns = 1 + (num_trackers / 2);

			for(int i = 0; i < num_spawns; i++) spawn_asteroid(w);
		}
	}

	// prune

	if (w->player) {
		if (w->player->_flags & EF_DESTROYED)
			w->player = 0;
	}

	prune(w->entities);

	// score

	if (w->score > w->hiscore) // todo: sound?
		w->hiscore = w->score;
}

mat44 make_proj_view(vec2 centre, float fov, float virtual_height) {
	float camera_height = virtual_height / (tan(deg_to_rad(fov) * 0.5f) * 2.0f);

	mat44 proj = perspective(deg_to_rad(fov), g_view_aspect, camera_height * 0.1f, camera_height * 2.0f);
	mat44 view = camera_look_at(vec3(-centre, camera_height), vec3(-centre, 0.0f), vec3(0, -1, 0));

	return proj * view;
}

void world_render(world* w, draw_context* dc) {
	float zoom = 0.9f;

	// camera

	w->proj_view = make_proj_view(w->camera_target, 90.0f, (360.0f + 20.0f) * zoom);

	frustum fr		= make_frustum(w->proj_view);
	vec4 draw_plane	= { 0, 0, 1, 0 };
	aabb2 field		= { (vec2)intersect_planes_3(fr.left(), fr.top(), draw_plane), (vec2)intersect_planes_3(fr.right(), fr.bottom(), draw_plane) };

	set_proj_view(w->proj_view);

	// arena

	vec2 p0 = w->limit.min;
	vec2 p3 = w->limit.max;
	vec2 p1 = { p3.x, p0.y };
	vec2 p2 = { p0.x, p3.y };

	colour c0 = colours::AQUA * colour(0.5f, 1.0f);
	colour c1 = colours::AQUA * colour(0.1f, 0.0f);

	int gh = 20;
	int gw = (int)(gh * (16.0f / 9.0f));

	for(int i = 1; i < gh; i++) {
		float f = i / (float)gh;
		dc->line(lerp(p0, p2, f), lerp(p1, p3, f), 0.5f, c1);
	}

	for(int i = 1; i < gw; i++) {
		float f = i / (float)gw;
		dc->line(lerp(p0, p1, f), lerp(p2, p3, f), 0.5f, c1);
	}

	dc->line(p0, p1, 1.0f, c0);
	dc->line(p1, p3, 1.0f, c0);
	dc->line(p3, p2, 1.0f, c0);
	dc->line(p2, p0, 1.0f, c0);

	// entities

	for(auto e : w->entities) if (e->_flags & EF_BOMB) e->render(dc);

	psys_render(dc);

	for(auto e : w->entities) if (e->_flags & EF_PICKUP) e->render(dc);
	for(auto e : w->entities) if (e->_flags & EF_BULLET) e->render(dc);
	for(auto e : w->entities) if (e->_flags & EF_ENEMY) e->render(dc);

	if (w->player)
		w->player->render(dc);
	
	// mouse

	if (gUsingMouse) {
		dc->push_transform(translate(vec3(g_mouse_screen, 0.0f)));

		float _gap[2] = { 8.5f, 8.0f };
		float _siz[2] = { 4.5f, 4.0f };
		float _thi[2] = { 3.0f, 2.0f };
		colour _c[2] = { colour(0.0f, 1.0f), colour(1.0f) };

		for(int i = 0; i < 2; i++) {
			float gap = _gap[i];
			float siz = _siz[i];
			float thi = _thi[i];

			dc->rect({-siz, -gap }, { siz, -gap + thi }, _c[i]);
			dc->rect({-siz, gap - thi }, { siz, gap }, _c[i]);

			dc->rect({ -gap, -siz }, { -gap + thi, siz }, _c[i]);
			dc->rect({ gap - thi, -siz }, { gap, siz }, _c[i]);

			dc->rect(-1.0f, 1.0f, _c[i]);
		}

		dc->pop_transform();
	}
}

// helpers

entity* spawn_entity(world* w, entity* ent) {
	if (ent) {
		ent->_world = w;

		w->entities.push_back(ent);

		if (ent->_type == ET_PLAYER) {
			w->player = (player*)ent;
		}

		ent->init();
	}

	return ent;
}

player* find_nearest_player(world* w, vec2 p) {
	return w->player;
}

bool within(entity* a, entity* b, float d) {
	return a && b && (length_sq(a->_pos - b->_pos) < square(d));
}

player* overlaps_player(entity* e) {
	if (!e)
		return 0;

	player* p = find_nearest_player(e->_world, e->_pos);

	if (!p)
		return 0;

	float d = e->_radius + p->_radius * 0.5f;

	if (within(p, e, d))
		return p;

	return 0;
}

unit* find_enemy_near_line(world* w, vec2 from, vec2 to, float r) {
	unit* best_u = 0;
	float best_t = FLT_MAX;

	vec2	d = to - from;
	float	l = length(d);

	d /= l;

	for(auto e : w->entities) {
		if (e->_flags & (EF_DESTROYED | EF_SPAWNING))
			continue;

		if (!(e->_flags & EF_UNIT))
			continue;

		if (!(e->_flags & EF_ENEMY))
			continue;

		float	t = clamp(dot((e->_pos - from), d), 0.0f, l);
		vec2	p = from + d * t;
		float	s = length_sq(e->_pos - p);

		if (s < square(e->_radius + r)) {
			if (t < best_t) {
				best_u = (unit*)e;
				best_t = t;
			}
		}
	}

	return best_u;
}

void avoid_crowd(world* w, entity* self, bool asteroids_too) {
	for(auto e : w->entities) {
		if (e == self)
			continue;

		if (e->_flags & EF_DESTROYED)
			continue;

		if (e->_type != self->_type) {
			if (!asteroids_too || (e->_type != ET_ASTEROID))
				continue;
		}

		vec2	d		= self->_pos - e->_pos;
		float	min_l	= self->_radius + e->_radius;

		if (length_sq(d) > square(min_l))
			continue;

		float l = length(d);

		if (l < 0.0001f)
			d = vec2(1.0f, 0.0f);
		else
			d /= l;

		float force_self	= 8.0f;
		float force_e		= 8.0f;

		if (asteroids_too && (e->_type == ET_ASTEROID)) {
			force_self	= 64.0f;
			force_e		= 1.0f;
		}

		self->_vel += d * force_self;
		e->_vel -= d * force_e;

		if (!asteroids_too) {
			if ((self->_type == ET_ASTEROID) && (e->_type == ET_ASTEROID)) {
				asteroid* aa = (asteroid*)self;
				asteroid* bb = (asteroid*)e;

				vec2 n = normalise(d);

				aa->_rot_v += dot(n, aa->_vel) * DT;
				bb->_rot_v += dot(-n, bb->_vel) * DT;
			}
		}
	}
}

unit* find_overlapping_unit(world* w, vec2 pos, entity_type type, float r) {
	unit* best_u = 0;
	float best_d = FLT_MAX;

	for(auto e : w->entities) {
		if (e->_flags & (EF_DESTROYED | EF_SPAWNING))
			continue;

		if (!(e->_flags & EF_UNIT))
			continue;

		if (e->_type != type)
			continue;

		float d = length_sq(e->_pos - pos);

		if (d < square(e->_radius + r)) {
			d = sqrtf(d) - e->_radius;

			if (d < best_d) {
				best_u = (unit*)e;
				best_d = d;
			}
		}
	}

	return best_u;
}