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
	EF_ENEMY			= 0x10,
	EF_SPAWNING			= 0x20,
	EF_UNIT				= 0x40,
	EF_PICKUP			= 0x80,
	EF_BOMB				= 0x100
};

enum entity_type {
	ET_PLAYER,
	ET_BULLET,
	ET_ASTEROID,
	ET_MINI_ASTEROID,
	ET_SHOOTING_STAR,
	ET_TRACKER,
	ET_DODGER,
	ET_PICKUP,
	ET_BOMB
};

struct entity {
	entity(entity_type type);
	virtual ~entity();

	void destroy();
	void update();
	virtual void render(draw_context* dc);

	virtual void init();
	virtual void tick();
	virtual void post_tick();
	virtual void hit_wall(int clipped);
	virtual void draw(draw_context* dc);

	world* _world;
	u32 _flags;
	entity_type _type;
	vec2 _pos;
	vec2 _old_pos;
	vec2 _vel;
	float _radius;
	colour _colour;
	float _rot;
	int _spawn_timer;
};

enum class damage_type {
	BULLET,
	COLLISION,
	PUSH,
	BOMB
};

struct damage_desc {
	damage_desc(damage_type type_, int value_, entity* cause_) : type(type_), value(value_), cause(cause_) { }

	damage_type type;
	int value;
	entity* cause;
};

struct unit : entity {
	unit(entity_type type);

	void instant_spawn();

	virtual void post_tick();

	virtual void damage(damage_desc* dd);
	virtual void flinch(damage_desc* dd);
	virtual void killed(damage_desc* dd);

	int _health;
};

struct player : unit {
	player();

	virtual void init();
	virtual void tick();
	virtual void post_tick();
	virtual void draw(draw_context* dc);

	virtual void damage(damage_desc* dd);
	virtual void flinch(damage_desc* dd);
	virtual void killed(damage_desc* dd);

	void try_fire(vec2 dir);

	int _reload_time;
	int _health_recharge;
	int _shield_time;
	int _recharge_pulse;
	float _rot_v;
	bool _tada;
};

struct bullet : entity {
	bullet();

	virtual void tick();
	virtual void post_tick();
	virtual void hit_wall(int clipped);
	virtual void render(draw_context* dc);

	int _time;
};

struct bomb : entity {
	bomb();

	virtual void tick();
	virtual void post_tick();
	virtual void hit_wall(int clipped);
	virtual void draw(draw_context* dc);

	int _time;
};

struct pickup : entity {
	pickup();

	virtual void init();
	virtual void tick();
	virtual void post_tick();
	virtual void hit_wall(int clipped);
	virtual void draw(draw_context* dc);

	int _time;
};

struct tracker : unit {
	tracker();

	virtual void init();
	virtual void tick();
	virtual void post_tick();
	virtual void hit_wall(int clipped);
	virtual void draw(draw_context* dc);

	virtual void damage(damage_desc* dd);

	int _drift_time;
	vec2 _drift_force;
	int _dizzy;
};

struct shooting_star : unit {
	shooting_star();

	virtual void init();
	virtual void tick();
	virtual void post_tick();
	virtual void hit_wall(int clipped);
	virtual void draw(draw_context* dc);

	virtual void damage(damage_desc* dd);

	int _flash;
	vec2 _desired_vel;
	bool _tada;
};

#define NUM_AST_PROFILE 12

struct asteroid : unit {
	asteroid();

	virtual void init();
	virtual void tick();
	virtual void post_tick();
	virtual void hit_wall(int clipped);
	virtual void draw(draw_context* dc);

	virtual void damage(damage_desc* dd);

	float _profile[NUM_AST_PROFILE];
	float _rot_v;
	int _flash;
};

struct mini_asteroid : unit {
	mini_asteroid();

	virtual void tick();
	virtual void post_tick();
	virtual void hit_wall(int clipped);

	virtual void damage(damage_desc* dd);
};

// world

struct world {
	random r;
	aabb2 limit;
	aabb2 outer_limit;

	player* player;
	list<entity> entities;

	mat44 proj_view;
	vec2 camera_lerp;
	vec2 camera_target;
	float shake;

	int spawn_time;
	int level_time;
	int kills;

	u64 score;
	u64 multi;

	u64 hiscore;
	bool had_hiscore;

	world();
};

entity* spawn_entity(world* w, entity* ent);
template<typename T> T* spawn_entity(world* w, T* ent) { return (T*)spawn_entity(w, (entity*)ent); }

void world_init(world* w);
void world_clear(world* w);
void world_update(world* w);
void world_render(world* w, draw_context* dc);

extern world g_world;

// helpers

player* find_nearest_player(world* w, vec2 p);
bool within(entity* a, entity* b, float d);
player* overlaps_player(entity* e);
unit* find_enemy_near_line(world* w, vec2 from, vec2 to, float r);
void avoid_crowd(world* w, entity* self, bool asteroids_too);
unit* find_overlapping_unit(world* w, vec2 pos, entity_type type, float r);

void spawn_shooting_star(world* w, entity* instigator);
void spawn_asteroid(world* w);
void spawn_mini_asteroids(world* w, vec2 pos);
void spawn_pickup(world* w, vec2 pos);
void spawn_bomb(world* w, vec2 pos);

// menu

bool menu_update();
void menu_render(draw_context* dc);

// particles

void psys_init(int max_particles);
void psys_update();
void psys_render(draw_context* dc);
void psys_spawn(vec2 pos, vec2 vel, float damp, float size0, float size1, float rot_v, colour c, int lifetime);

// effects

void fx_explosion(vec2 pos, float strength, int count, colour c, float psize);

#endif // GAME_H