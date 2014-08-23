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

world::world() : player(), spawn_time(), level_time() {
}

void world_init(world* w) {
	w->limit		= { { -320.0f, -180.0f }, { 320.0f, 180.0f } };
	w->outer_limit	= inflate(w->limit, 80.0f);

	w->player = spawn_entity(w, new player);
}

void world_update(world* w) {
	g_mouse_screen = (vec2)unproject(vec3(to_vec2(gMousePos), 0.0f), inverse(w->proj_view), to_vec2(g_view_size));

	// update

	// todo: players and bullets first, then enemies? possibly post spawn too?

	for_all(w->entities, [](entity* e) { e->update(); });

	// spawn

	w->level_time++;

	int t = max(60 - (int)sqrt(w->level_time / 2), 0);

	if (++w->spawn_time > t) {
		tracker* t = spawn_entity(w, new tracker);

		vec2 p = w->r.range(w->limit.min + t->_radius, w->limit.max - t->_radius);

		t->_pos = p;
		t->_old_pos = p;

		w->spawn_time = 0;
	}

	// prune

	prune(w->entities);
}

mat44 make_proj_view(vec2 centre, float fov, float virtual_height) {
	float camera_height = virtual_height / (tan(deg_to_rad(fov) * 0.5f) * 2.0f);

	mat44 proj = perspective(deg_to_rad(fov), g_view_aspect, camera_height * 0.1f, camera_height * 2.0f);
	mat44 view = camera_look_at(vec3(-centre, camera_height), vec3(-centre, 0.0f), vec3(0, -1, 0));

	return proj * view;
}

void world_render(world* w, draw_context* dc) {
	vec2 camera_target;
	float zoom = 0.9f;

	if (w->player) {
		camera_target = w->player->_pos * 0.25f;
	}

	// camera

	w->proj_view = make_proj_view(camera_target, 90.0f, (360.0f + 20.0f) * zoom);

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

	for(auto e : w->entities) if (e->_flags & EF_BULLET) e->render(dc);
	for(auto e : w->entities) if (e->_flags & EF_ENEMY) e->render(dc);

	if (w->player)
		w->player->render(dc);
	
	// mouse

	if (gUsingMouse) dc->rect(g_mouse_screen - 2, g_mouse_screen + 2, colour());
}

// helpers

entity* spawn_entity(world* w, entity* ent) {
	if (ent) {
		ent->_world = w;
		w->entities.push_back(ent);
	}

	return ent;
}

player* find_nearest_player(world* w, vec2 p) {
	return w->player;
}

bool within(entity* a, entity* b, float d) {
	return a && b && (length_sq(a->_pos - b->_pos) < square(d));
}

bool overlaps_player(entity* e) {
	if (!e)
		return false;

	entity* p = find_nearest_player(e->_world, e->_pos);

	if (!p)
		return false;

	float d = e->_radius + p->_radius * 0.5f;

	return within(p, e, d);
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

void avoid_crowd(world* w, entity* self) {
	for(auto e : w->entities) {
		if (e == self)
			continue;

		if (e->_flags & EF_DESTROYED)
			continue;

		if (e->_type != self->_type)
			continue;

		vec2	d		= self->_pos - e->_pos;
		float	min_l	= self->_radius + e->_radius;

		if (length_sq(d) > square(min_l))
			continue;

		float l = length(d);

		if (l < 0.0001f)
			d = vec2(1.0f, 0.0f);
		else
			d /= l;

		d *= 8.0f;

		self->_vel += d;
		e->_vel -= d;
	}
}