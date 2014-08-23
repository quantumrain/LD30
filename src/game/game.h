#ifndef GAME_H
#define GAME_H

#include "tile_map.h"

struct world;

// entities

enum entity_flag {
	EF_DESTROYED		= 0x1,
	EF_USE_OUTER_LIMIT	= 0x2,
	EF_PLAYER			= 0x4,
	EF_BULLET			= 0x8,
	EF_ENEMY			= 0x10
};

enum entity_type {
	ET_PLAYER,
	ET_BULLET,
	ET_ASTEROID,
	ET_MINI_ASTEROID,
	ET_SHOOTING_STAR,
	ET_TRACKER,
	ET_DODGER
};

struct entity {
	entity(entity_type type);
	virtual ~entity();

	void destroy();
	void update();
	void render(draw_context* dc);

	virtual void tick();
	virtual void post_tick();
	virtual void hit_wall(int side);
	virtual void draw(draw_context* dc);

	world* _world;
	u32 _flags;
	entity_type _type;
	vec2 _pos;
	vec2 _old_pos;
	vec2 _vel;
	float _radius;
	colour _colour;
};

struct player : entity {
	player();

	virtual void tick();
	virtual void post_tick();

	void try_fire(vec2 dir);

	int _reload_time;
};

struct bullet : entity {
	bullet();

	virtual void tick();
	virtual void post_tick();
	virtual void hit_wall(int side);

	int _time;
};

struct tracker : entity {
	tracker();

	virtual void tick();
	virtual void post_tick();
	virtual void hit_wall(int side);
};

struct shooting_star : entity {
	shooting_star();

	virtual void tick();
	virtual void post_tick();
	virtual void hit_wall(int side);
};

// world

struct world {
	random r;
	aabb2 limit;
	aabb2 outer_limit;
	player* player;
	list<entity> entities;

	int spawn_time;
	int level_time;

	world();
};

entity* spawn_entity(world* w, entity* ent);
template<typename T> T* spawn_entity(world* w, T* ent) { return (T*)spawn_entity(w, (entity*)ent); }

void world_init(world* w);
void world_update(world* w);
void world_render(world* w, draw_context* dc);

extern world g_world;

// helpers

player* find_nearest_player(world* w, vec2 p);
bool within(entity* a, entity* b, float d);
bool overlaps_player(entity* e);
entity* find_enemy_near_line(world* w, vec2 from, vec2 to, float r);

void spawn_shooting_star(world* w, entity* instigator);

#endif // GAME_H