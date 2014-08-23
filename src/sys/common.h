#ifndef COMMON_H
#define COMMON_H

#include "ut.h"
#include "vec.h"
#include "render.h"

struct random {
	random();
	explicit random(uint64_t seed);

	void set_seed(uint64_t seed);

	random gen();

	uint64_t rand64();
	double frand64();

	bool chance(int n, int out_of);

	int rand(int max);
	int range(int min, int max);
	int range(int size);

	float rand(float max);
	float range(float min, float max);
	float range(float size);

	vec2 rand(const vec2& max);
	vec2 range(const vec2& min, const vec2& max);
	vec2 range(const vec2& size);

	vec3 rand(const vec3& max);
	vec3 range(const vec3& min, const vec3& max);
	vec3 range(const vec3& size);

	vec4 rand(const vec4& max);
	vec4 range(const vec4& min, const vec4& max);
	vec4 range(const vec4& size);

	vec2i rand(const vec2i& max);
	vec2i range(const vec2i& min, const vec2i& max);
	vec2i range(const vec2i& size);

	vec3i rand(const vec3i& max);
	vec3i range(const vec3i& min, const vec3i& max);
	vec3i range(const vec3i& size);

	vec4i rand(const vec4i& max);
	vec4i range(const vec4i& min, const vec4i& max);
	vec4i range(const vec4i& size);

	uint64_t _a, _b;
};

int hash(const uint8_t* data, int size);
float gaussian(float n, float theta);
bool overlaps_rect(vec2 c0, vec2 s0, vec2 c1, vec2 s1);

enum key_press {
	KEY_NONE,
	KEY_L_UP,
	KEY_L_DOWN,
	KEY_L_LEFT,
	KEY_L_RIGHT,
	KEY_R_UP,
	KEY_R_DOWN,
	KEY_R_LEFT,
	KEY_R_RIGHT,
	KEY_FIRE,
	KEY_ALT_FIRE,
	KEY_1,
	KEY_2,
	KEY_3,
	KEY_4,
	KEY_5,
	KEY_6,
	KEY_7,
	KEY_8,
	KEY_9,
	KEY_0,
	KEY_RESET,
	KEY_CHEAT,
	KEY_MODE,
	KEY_PLACE,
	KEY_MAX
};

extern int g_LocKeyW;
extern int g_LocKeyS;
extern int g_LocKeyA;
extern int g_LocKeyD;

extern int g_LocKeyZ;
extern int g_LocKeyX;
extern int g_LocKeyC;

extern int g_LocKeyI;
extern int g_LocKeyO;
extern int g_LocKeyP;

extern vec2i g_view_size;
extern float g_view_aspect;

extern DWORD gKeyDown[KEY_MAX];
extern vec2i gMousePos;
extern int gMouseButtons;
extern int gMouseTime;
extern bool gUsingMouse;

extern vec2 gJoyLeft;
extern vec2 gJoyRight;
extern bool gJoyA;

bool is_key_down(key_press k);

// Debug

void debug(const char* txt, ...);
void panic(const char* txt, ...);

// Sound

enum class sound_id {
	DIT,
	MAX
};

void SoundInit();
void SoundShutdown();
void SoundPlay(sound_id sid, float freq, float volume);

// File

struct file_buf {
	uint8_t* data;
	int size;

	file_buf() : data(), size() { }
	~file_buf() { destroy(); }

	void destroy() { delete [] data; data = 0; size = 0; }
};

bool load_file(file_buf* fb, const char* path);
texture* load_texture(const char* path);

// timer

uint64_t timer_ticks();
uint64_t timer_freq();
double timer_ticks_to_secs(uint64_t ticks);

#endif // COMMON_H