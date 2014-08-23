#include "pch.h"
#include "common.h"

#define STBI_NO_STDIO
#include "stb_image.c"

extern HWND gMainWnd;
extern const wchar_t* gAppName;

void debug(const char* txt, ...) {
	char buf[512];

	va_list ap;

	va_start(ap, txt);
	_vsnprintf_s(buf, sizeof(buf), _TRUNCATE, txt, ap);
	va_end(ap);

	OutputDebugStringA(buf);
	OutputDebugStringA("\r\n");
}

void panic(const char* txt, ...) {
	char buf[512];

	va_list ap;

	va_start(ap, txt);
	_vsnprintf_s(buf, sizeof(buf), _TRUNCATE, txt, ap);
	va_end(ap);

	char title[256];
	WideCharToMultiByte(CP_UTF8, 0, gAppName, -1, title, 256, 0, FALSE);

	MessageBoxA(gMainWnd, buf, title, MB_ICONERROR | MB_OK);
	ExitProcess(0);
}

int hash(const uint8_t* data, int size) {
	int h = 5381;

	while(size-- > 0)
		h = h * 33 + *data;

	return h;
}

float gaussian(float n, float theta) {
	return ((1.0f / sqrtf(2.0f * 3.14159f * theta)) * expf(-(n * n) / (2.0f * theta * theta)));   
}

bool overlaps_rect(vec2 c0, vec2 s0, vec2 c1, vec2 s1)
{
	vec2 delta = c1 - c0;
	vec2 size = (s0 + s1) * 0.5f;
	
	return (fabsf(delta.x) < size.x) && (fabsf(delta.y) < size.y);
}

bool load_file(file_buf* fb, const char* path) {
	fb->destroy();

	HANDLE h = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);

	if (h == INVALID_HANDLE_VALUE)
		return false;

	DWORD size = GetFileSize(h, 0);

	if (size < (64 * 1024 * 1024)) {
		if ((fb->data = new uint8_t[size]) != 0) {
			fb->size = (int)size;
			DWORD bytes = 0;
			if (!ReadFile(h, fb->data, size, &bytes, 0) || (bytes != size)) {
				fb->destroy();
			}
		}
	}

	CloseHandle(h);

	return fb->data != 0;
}

texture* load_texture(const char* path) {
	file_buf fb;

	if (!load_file(&fb, path))
		return 0;

	int width = 0, height = 0;
	uint8_t* data = stbi_load_from_memory(fb.data, fb.size, &width, &height, 0, 4);

	texture* tex = CreateTexture(width, height, gpu_format::RGBA, data);

	free(data);

	return tex;
}

// random - based on tinymt64

#define MAT1 0xFA051F40ull
#define MAT2 0xFFD0FFF4ull
#define TMAT 0x58D02FFEFFBFFFBCull

random::random() { set_seed(0); }
random::random(uint64_t seed) { set_seed(seed); }

void random::set_seed(uint64_t seed) {
	_a = seed ^ (MAT1 << 32);
	_b = MAT2 ^ TMAT;

	_b ^= 1 + TMAT * (_a ^ (_a >> 62));
	_a ^= 2 + TMAT * (_b ^ (_b >> 62));
	_b ^= 3 + TMAT * (_a ^ (_a >> 62));
	_a ^= 4 + TMAT * (_b ^ (_b >> 62));
	_b ^= 5 + TMAT * (_a ^ (_a >> 62));
	_a ^= 6 + TMAT * (_b ^ (_b >> 62));
	_b ^= 7 + TMAT * (_a ^ (_a >> 62));
	_a ^= 8 + TMAT * (_b ^ (_b >> 62));
}

random random::gen() {
	return random(rand64());
}

uint64_t random::rand64() {
	uint64_t x = (_a & 0x7FFFFFFFFFFFFFFFull) ^ _b;

	x ^= x << 12;
	x ^= x >> 32;
	x ^= x << 32;
	x ^= x << 11;

	_a = _b ^ (-(int64_t)(x & 1)) & MAT1;
	_b = x ^ (-(int64_t)(x & 1)) & (MAT2 << 32);

	uint64_t y = (_a + _b) ^ (_a >> 8);

	return y ^ ((-(int64_t)(y & 1)) & TMAT);
}

double random::frand64() {
	return rand64() * (1.0 / 18446744073709551616.0);
}

bool random::chance(int n, int out_of)					{ return (rand64() % out_of) < n; }

int random::rand(int max)								{ return (max > 0) ? (rand64() % max) : 0; }
int random::range(int min, int max)						{ return (min < max) ? (min + (rand64() % (max - min + 1))) : min; }
int random::range(int size)								{ return range(-size, size); }

float random::rand(float max)							{ return (float)frand64() * max; }
float random::range(float min, float max)				{ return (float)(min + frand64() * (max - min)); }
float random::range(float size)							{ return (float)(frand64() * size * 2.0f - size);	}

vec2 random::rand(const vec2& max)						{ return vec2(rand(max.x), rand(max.y)); }
vec2 random::range(const vec2& min, const vec2& max)	{ return vec2(range(min.x, max.x), range(min.y, max.y)); }
vec2 random::range(const vec2& size)					{ return vec2(range(size.x), range(size.y)); }

vec3 random::rand(const vec3& max)						{ return vec3(rand(max.x), rand(max.y), rand(max.z)); }
vec3 random::range(const vec3& min, const vec3& max)	{ return vec3(range(min.x, max.x), range(min.y, max.y), range(min.z, max.z)); }
vec3 random::range(const vec3& size)					{ return vec3(range(size.x), range(size.y), range(size.z)); }

vec4 random::rand(const vec4& max)						{ return vec4(rand(max.x), rand(max.y), rand(max.z), rand(max.w)); }
vec4 random::range(const vec4& min, const vec4& max)	{ return vec4(range(min.x, max.x), range(min.y, max.y), range(min.z, max.z), range(min.w, max.w)); }
vec4 random::range(const vec4& size)					{ return vec4(range(size.x), range(size.y), range(size.z), range(size.w)); }

vec2i random::rand(const vec2i& max)					{ return vec2i(rand(max.x), rand(max.y)); }
vec2i random::range(const vec2i& min, const vec2i& max)	{ return vec2i(range(min.x, max.x), range(min.y, max.y)); }
vec2i random::range(const vec2i& size)					{ return vec2i(range(size.x), range(size.y)); }

vec3i random::rand(const vec3i& max)					{ return vec3i(rand(max.x), rand(max.y), rand(max.z)); }
vec3i random::range(const vec3i& min, const vec3i& max)	{ return vec3i(range(min.x, max.x), range(min.y, max.y), range(min.z, max.z)); }
vec3i random::range(const vec3i& size)					{ return vec3i(range(size.x), range(size.y), range(size.z)); }

vec4i random::rand(const vec4i& max)					{ return vec4i(rand(max.x), rand(max.y), rand(max.z), rand(max.w)); }
vec4i random::range(const vec4i& min, const vec4i& max)	{ return vec4i(range(min.x, max.x), range(min.y, max.y), range(min.z, max.z), range(min.w, max.w)); }
vec4i random::range(const vec4i& size)					{ return vec4i(range(size.x), range(size.y), range(size.z), range(size.w)); }
