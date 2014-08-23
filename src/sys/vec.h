#ifndef VEC_H
#define VEC_H

template<typename T> struct vec2t;
template<typename T> struct vec3t;
template<typename T> struct vec4t;

template<typename T>
struct vec2t {
	T x, y;

	vec2t() : x(), y() { }
	vec2t(T xy) : x(xy), y(xy) { }
	vec2t(T x_, T y_) : x(x_), y(y_) { }

	T& operator[](int i) { assert(i >= 0 && i < 2); return (&x)[i]; }
};

template<typename T>
struct vec3t {
	T x, y, z;

	vec3t() : x(), y(), z() { }
	vec3t(T xyz) : x(xyz), y(xyz), z(xyz) { }
	vec3t(T x_, T y_, T z_) : x(x_), y(y_), z(z_) { }
	vec3t(const vec2t<T>& xy, T z_) : x(xy.x), y(xy.y), z(z_) { }

	explicit operator vec2t<T>() const { return vec2t<T>(x, y); }

	T& operator[](int i) { assert(i >= 0 && i < 3); return (&x)[i]; }
};

template<typename T>
struct vec4t {
	T x, y, z, w;

	vec4t() : x(), y(), z(), w() { }
	vec4t(T xyzw) : x(xyzw), y(xyzw), z(xyzw), w(xyzw) { }
	vec4t(T x_, T y_, T z_, T w_) : x(x_), y(y_), z(z_), w(w_) { }
	vec4t(const vec2t<T>& xy, T z_, T w_) : x(xy.x), y(xy.y), z(z_), w(w_) { }
	vec4t(const vec2t<T>& xy, const vec2t<T>& zw) : x(xy.x), y(xy.y), z(zw.x), w(zw.y) { }
	vec4t(const vec3t<T>& xyz, T w_) : x(xyz.x), y(xyz.y), z(xyz.z), w(w_) { }

	explicit operator vec2t<T>() const { return vec2t<T>(x, y); }
	explicit operator vec3t<T>() const { return vec3t<T>(x, y, z); }

	T& operator[](int i) { assert(i >= 0 && i < 4); return (&x)[i]; }
};

typedef vec2t<float> vec2;
typedef vec3t<float> vec3;
typedef vec4t<float> vec4;

typedef vec2t<i32> vec2i;
typedef vec3t<i32> vec3i;
typedef vec4t<i32> vec4i;

struct mat44 {
	vec4 x, y, z, w;

	mat44() : x(1.0f, 0.0f, 0.0f, 0.0f), y(0.0f, 1.0f, 0.0f, 0.0f), z(0.0f, 0.0f, 1.0f, 0.0f), w(0.0f, 0.0f, 0.0f, 1.0f) { }
	mat44(const vec4& x_, const vec4& y_, const vec4& z_, const vec4& w_) : x(x_), y(y_), z(z_), w(w_) { }
	mat44(float m00, float m01, float m02, float m03, float m10, float m11, float m12, float m13, float m20, float m21, float m22, float m23, float m30, float m31, float m32, float m33) : x(m00, m01, m02, m03), y(m10, m11, m12, m13), z(m20, m21, m22, m23), w(m30, m31, m32, m33) { }
};

struct frustum {
	vec4 planes[6];

	enum { LEFT, RIGHT, TOP, BOTTOM, NEAR, FAR };

	const vec4& left() const { return planes[LEFT]; }
	const vec4& right() const { return planes[RIGHT]; }
	const vec4& top() const { return planes[TOP]; }
	const vec4& bottom() const { return planes[BOTTOM]; }
	const vec4& near() const { return planes[NEAR]; }
	const vec4& far() const { return planes[FAR]; }
};

inline vec2 operator+(const vec2& lhs, const vec2& rhs) { return vec2(lhs.x + rhs.x, lhs.y + rhs.y); }
inline vec2 operator-(const vec2& lhs, const vec2& rhs) { return vec2(lhs.x - rhs.x, lhs.y - rhs.y); }
inline vec2 operator*(const vec2& lhs, const vec2& rhs) { return vec2(lhs.x * rhs.x, lhs.y * rhs.y); }
inline vec2 operator/(const vec2& lhs, const vec2& rhs) { return vec2(lhs.x / rhs.x, lhs.y / rhs.y); }

inline vec2 operator-(const vec2& rhs) { return vec2(-rhs.x, -rhs.y); }

inline vec2& operator+=(vec2& lhs, const vec2& rhs) { lhs.x += rhs.x; lhs.y += rhs.y; return lhs; }
inline vec2& operator-=(vec2& lhs, const vec2& rhs) { lhs.x -= rhs.x; lhs.y -= rhs.y; return lhs; }
inline vec2& operator*=(vec2& lhs, const vec2& rhs) { lhs.x *= rhs.x; lhs.y *= rhs.y; return lhs; }
inline vec2& operator/=(vec2& lhs, const vec2& rhs) { lhs.x /= rhs.x; lhs.y /= rhs.y; return lhs; }

inline vec3 operator+(const vec3& lhs, const vec3& rhs) { return vec3(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z); }
inline vec3 operator-(const vec3& lhs, const vec3& rhs) { return vec3(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z); }
inline vec3 operator*(const vec3& lhs, const vec3& rhs) { return vec3(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z); }
inline vec3 operator/(const vec3& lhs, const vec3& rhs) { return vec3(lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z); }

inline vec3 operator-(const vec3& rhs) { return vec3(-rhs.x, -rhs.y, -rhs.z); }

inline vec3& operator+=(vec3& lhs, const vec3& rhs) { lhs.x += rhs.x; lhs.y += rhs.y; lhs.z += rhs.z; return lhs; }
inline vec3& operator-=(vec3& lhs, const vec3& rhs) { lhs.x -= rhs.x; lhs.y -= rhs.y; lhs.z -= rhs.z; return lhs; }
inline vec3& operator*=(vec3& lhs, const vec3& rhs) { lhs.x *= rhs.x; lhs.y *= rhs.y; lhs.z *= rhs.z; return lhs; }
inline vec3& operator/=(vec3& lhs, const vec3& rhs) { lhs.x /= rhs.x; lhs.y /= rhs.y; lhs.z /= rhs.z; return lhs; }

inline vec4 operator+(const vec4& lhs, const vec4& rhs) { return vec4(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, lhs.w + rhs.w); }
inline vec4 operator-(const vec4& lhs, const vec4& rhs) { return vec4(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z, lhs.w - rhs.w); }
inline vec4 operator*(const vec4& lhs, const vec4& rhs) { return vec4(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z, lhs.w * rhs.w); }
inline vec4 operator/(const vec4& lhs, const vec4& rhs) { return vec4(lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z, lhs.w / rhs.w); }

inline vec4 operator-(const vec4& rhs) { return vec4(-rhs.x, -rhs.y, -rhs.z, -rhs.w); }

inline vec4& operator+=(vec4& lhs, const vec4& rhs) { lhs.x += rhs.x; lhs.y += rhs.y; lhs.z += rhs.z; lhs.w += rhs.w; return lhs; }
inline vec4& operator-=(vec4& lhs, const vec4& rhs) { lhs.x -= rhs.x; lhs.y -= rhs.y; lhs.z -= rhs.z; lhs.w -= rhs.w; return lhs; }
inline vec4& operator*=(vec4& lhs, const vec4& rhs) { lhs.x *= rhs.x; lhs.y *= rhs.y; lhs.z *= rhs.z; lhs.w *= rhs.w; return lhs; }
inline vec4& operator/=(vec4& lhs, const vec4& rhs) { lhs.x /= rhs.x; lhs.y /= rhs.y; lhs.z /= rhs.z; lhs.w /= rhs.w; return lhs; }

inline vec2i operator+(const vec2i& lhs, const vec2i& rhs) { return vec2i(lhs.x + rhs.x, lhs.y + rhs.y); }
inline vec2i operator-(const vec2i& lhs, const vec2i& rhs) { return vec2i(lhs.x - rhs.x, lhs.y - rhs.y); }
inline vec2i operator*(const vec2i& lhs, const vec2i& rhs) { return vec2i(lhs.x * rhs.x, lhs.y * rhs.y); }
inline vec2i operator/(const vec2i& lhs, const vec2i& rhs) { return vec2i(lhs.x / rhs.x, lhs.y / rhs.y); }

inline vec2i operator-(const vec2i& rhs) { return vec2i(-rhs.x, -rhs.y); }

inline vec2i& operator+=(vec2i& lhs, const vec2i& rhs) { lhs.x += rhs.x; lhs.y += rhs.y; return lhs; }
inline vec2i& operator-=(vec2i& lhs, const vec2i& rhs) { lhs.x -= rhs.x; lhs.y -= rhs.y; return lhs; }
inline vec2i& operator*=(vec2i& lhs, const vec2i& rhs) { lhs.x *= rhs.x; lhs.y *= rhs.y; return lhs; }
inline vec2i& operator/=(vec2i& lhs, const vec2i& rhs) { lhs.x /= rhs.x; lhs.y /= rhs.y; return lhs; }

inline vec3i operator+(const vec3i& lhs, const vec3i& rhs) { return vec3i(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z); }
inline vec3i operator-(const vec3i& lhs, const vec3i& rhs) { return vec3i(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z); }
inline vec3i operator*(const vec3i& lhs, const vec3i& rhs) { return vec3i(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z); }
inline vec3i operator/(const vec3i& lhs, const vec3i& rhs) { return vec3i(lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z); }

inline vec3i operator-(const vec3i& rhs) { return vec3i(-rhs.x, -rhs.y, -rhs.z); }

inline vec3i& operator+=(vec3i& lhs, const vec3i& rhs) { lhs.x += rhs.x; lhs.y += rhs.y; lhs.z += rhs.z; return lhs; }
inline vec3i& operator-=(vec3i& lhs, const vec3i& rhs) { lhs.x -= rhs.x; lhs.y -= rhs.y; lhs.z -= rhs.z; return lhs; }
inline vec3i& operator*=(vec3i& lhs, const vec3i& rhs) { lhs.x *= rhs.x; lhs.y *= rhs.y; lhs.z *= rhs.z; return lhs; }
inline vec3i& operator/=(vec3i& lhs, const vec3i& rhs) { lhs.x /= rhs.x; lhs.y /= rhs.y; lhs.z /= rhs.z; return lhs; }

inline vec4i operator+(const vec4i& lhs, const vec4i& rhs) { return vec4i(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, lhs.w + rhs.w); }
inline vec4i operator-(const vec4i& lhs, const vec4i& rhs) { return vec4i(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z, lhs.w - rhs.w); }
inline vec4i operator*(const vec4i& lhs, const vec4i& rhs) { return vec4i(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z, lhs.w * rhs.w); }
inline vec4i operator/(const vec4i& lhs, const vec4i& rhs) { return vec4i(lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z, lhs.w / rhs.w); }

inline vec4i operator-(const vec4i& rhs) { return vec4i(-rhs.x, -rhs.y, -rhs.z, -rhs.w); }

inline vec4i& operator+=(vec4i& lhs, const vec4i& rhs) { lhs.x += rhs.x; lhs.y += rhs.y; lhs.z += rhs.z; lhs.w += rhs.w; return lhs; }
inline vec4i& operator-=(vec4i& lhs, const vec4i& rhs) { lhs.x -= rhs.x; lhs.y -= rhs.y; lhs.z -= rhs.z; lhs.w -= rhs.w; return lhs; }
inline vec4i& operator*=(vec4i& lhs, const vec4i& rhs) { lhs.x *= rhs.x; lhs.y *= rhs.y; lhs.z *= rhs.z; lhs.w *= rhs.w; return lhs; }
inline vec4i& operator/=(vec4i& lhs, const vec4i& rhs) { lhs.x /= rhs.x; lhs.y /= rhs.y; lhs.z /= rhs.z; lhs.w /= rhs.w; return lhs; }

inline vec4 operator*(const mat44& a, const vec4& b) { return (a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w); }
inline mat44 operator*(const mat44& a, const mat44& b) { return mat44(a * b.x, a * b.y, a * b.z, a * b.w); }

inline bool operator==(vec2i& lhs, vec2i& rhs) { return lhs.x == rhs.x && lhs.y == rhs.y; }
inline bool operator!=(vec2i& lhs, vec2i& rhs) { return lhs.x != rhs.x || lhs.y != rhs.y; }

inline vec2i ifloor(vec2 v) { return vec2i(ifloor(v.x), ifloor(v.y)); }
inline vec3i ifloor(vec3 v) { return vec3i(ifloor(v.x), ifloor(v.y), ifloor(v.z)); }
inline vec4i ifloor(vec4 v) { return vec4i(ifloor(v.x), ifloor(v.y), ifloor(v.z), ifloor(v.w)); }

inline vec2 to_vec2(vec2i v) { return vec2((float)v.x, (float)v.y); }
inline vec3 to_vec2(vec3i v) { return vec3((float)v.x, (float)v.y, (float)v.z); }
inline vec4 to_vec2(vec4i v) { return vec4((float)v.x, (float)v.y, (float)v.z, (float)v.w); }

inline vec2 rotation(float a) { return vec2(cosf(a), sinf(a)); }
inline float rotation_of(const vec2& v) { return atan2f(v.y, v.x); }

inline vec2 min(const vec2& v0, const vec2& v1) { return vec2(min(v0.x, v1.x), min(v0.y, v1.y)); }
inline vec3 min(const vec3& v0, const vec3& v1) { return vec3(min(v0.x, v1.x), min(v0.y, v1.y), min(v0.z, v1.z)); }
inline vec4 min(const vec4& v0, const vec4& v1) { return vec4(min(v0.x, v1.x), min(v0.y, v1.y), min(v0.z, v1.z), min(v0.w, v1.w)); }

inline vec2 max(const vec2& v0, const vec2& v1) { return vec2(max(v0.x, v1.x), max(v0.y, v1.y)); }
inline vec3 max(const vec3& v0, const vec3& v1) { return vec3(max(v0.x, v1.x), max(v0.y, v1.y), max(v0.z, v1.z)); }
inline vec4 max(const vec4& v0, const vec4& v1) { return vec4(max(v0.x, v1.x), max(v0.y, v1.y), max(v0.z, v1.z), max(v0.w, v1.w)); }

inline float sum(const vec2& v) { return v.x + v.y; }
inline float sum(const vec3& v) { return v.x + v.y + v.z; }
inline float sum(const vec4& v) { return v.x + v.y + v.z + v.w; }

inline float dot(const vec2& v0, const vec2& v1) { return sum(v0 * v1); }
inline float dot(const vec3& v0, const vec3& v1) { return sum(v0 * v1); }
inline float dot(const vec4& v0, const vec4& v1) { return sum(v0 * v1); }

inline float length_sq(const vec2& v) { return sum(v * v); }
inline float length_sq(const vec3& v) { return sum(v * v); }
inline float length_sq(const vec4& v) { return sum(v * v); }

inline float length(const vec2& v) { return sqrtf(sum(v * v)); }
inline float length(const vec3& v) { return sqrtf(sum(v * v)); }
inline float length(const vec4& v) { return sqrtf(sum(v * v)); }

inline vec2 normalise(const vec2& v) { return v * vec2(1.0f / length(v)); }
inline vec3 normalise(const vec3& v) { return v * vec3(1.0f / length(v)); }
inline vec4 normalise(const vec4& v) { return v * vec4(1.0f / length(v)); }

inline vec2 perp(const vec2& v) { return vec2(-v.y, v.x); }
inline float cross(const vec2& v0, const vec2& v1) { return v0.x * v1.y - v1.x * v0.y; }
inline vec3 cross(const vec3& v0, const vec3& v1) { return vec3(v0.y * v1.z - v1.y * v0.z, v0.z * v1.x - v1.z * v0.x, v0.x * v1.y - v1.x * v0.y); }

mat44 perspective(float fov, float aspect, float z_near, float z_far);
mat44 perspective(float left, float right, float bottom, float top, float z_near, float z_far);
mat44 ortho(float left, float right, float bottom, float top, float z_near, float z_far);
mat44 look_at(const vec3& eye, const vec3& at, const vec3& up);
mat44 camera_look_at(const vec3& eye, const vec3& at, const vec3& up);
vec3 unproject(vec3 projected, mat44 inv_proj_view, vec2 viewport);

mat44 inverse(const mat44& m);
mat44 scale(const vec3& s);
mat44 rotate_x(float theta);
mat44 rotate_y(float theta);
mat44 rotate_z(float theta);
mat44 translate(const vec3& v);

enum class colours : u32 { 
	NAVY	= 0x001F3F, // from http://clrs.cc/
	BLUE	= 0x0074D9,
	AQUA	= 0x7FDBFF,
	TEAL	= 0x39CCCC,
	OLIVE	= 0x3D9970,
	GREEN	= 0x2ECC40,
	LIME	= 0x01FF70,
	YELLOW	= 0xFFDC00,
	ORANGE	= 0xFF851B,
	RED		= 0xFF4136,
	MAROON	= 0x85144B,
	FUCHSIA	= 0xF012BE,
	PURPLE	= 0xB10DC9,
	BLACK	= 0x111111,
	GREY	= 0xAAAAAA,
	SILVER	= 0xDDDDDD,
};

extern const colours COLOURS[16];

struct colour {
	float r, g, b, a;

	colour(float rgba = 1) : r(rgba), g(rgba), b(rgba), a(rgba) { }
	colour(float rgb, float a) : r(rgb), g(rgb), b(rgb), a(a) { }
	colour(float r_, float g_, float b_, float a_) : r(r_), g(g_), b(b_), a(a_) { }
	colour(vec4 v) : r(v.x), g(v.y), b(v.z), a(v.w) { }
	colour(colours c, float a_ = 1.0f) : r((((u32)c >> 16) & 0xFF) / 255.0f), g((((u32)c >> 8) & 0xFF) / 255.0f), b(((u32)c & 0xFF) / 255.0f), a(a_) { }
};

inline colour operator+(const colour& lhs, const colour& rhs) { return colour(lhs.r + rhs.r, lhs.g + rhs.g, lhs.b + rhs.b, lhs.a + rhs.a); }
inline colour operator-(const colour& lhs, const colour& rhs) { return colour(lhs.r - rhs.r, lhs.g - rhs.g, lhs.b - rhs.b, lhs.a - rhs.a); }
inline colour operator*(const colour& lhs, const colour& rhs) { return colour(lhs.r * rhs.r, lhs.g * rhs.g, lhs.b * rhs.b, lhs.a * rhs.a); }
inline colour operator/(const colour& lhs, const colour& rhs) { return colour(lhs.r / rhs.r, lhs.g / rhs.g, lhs.b / rhs.b, lhs.a / rhs.a); }

inline colour& operator+=(colour& lhs, const colour& rhs) { lhs.r += rhs.r; lhs.g += rhs.g; lhs.b += rhs.b; lhs.a += rhs.a; return lhs; }
inline colour& operator-=(colour& lhs, const colour& rhs) { lhs.r -= rhs.r; lhs.g -= rhs.g; lhs.b -= rhs.b; lhs.a -= rhs.a; return lhs; }
inline colour& operator*=(colour& lhs, const colour& rhs) { lhs.r *= rhs.r; lhs.g *= rhs.g; lhs.b *= rhs.b; lhs.a *= rhs.a; return lhs; }
inline colour& operator/=(colour& lhs, const colour& rhs) { lhs.r /= rhs.r; lhs.g /= rhs.g; lhs.b /= rhs.b; lhs.a /= rhs.a; return lhs; }

inline float luminance(const colour& c) { return c.r * 0.299f + c.g * 0.587f + c.b * 0.114f; }
inline colour saturation(const colour& c, float v) { return lerp(colour(luminance(c), c.a), c, v); }
inline colour lighten(const colour& c, float v) { return lerp(c, colour(1.0f, c.a), v); }
inline colour darken(const colour& c, float v) { return lerp(c, colour(0.0f, c.a), v); }

struct aabb2 {
	vec2 min;
	vec2 max;

	aabb2() { }
	aabb2(vec2 min_, vec2 max_) : min(min_), max(max_) { }
};

inline bool overlaps_x(const aabb2& a, const aabb2& b) { return (a.max.x > b.min.x) && (a.min.x < b.max.x); }
inline bool overlaps_y(const aabb2& a, const aabb2& b) { return (a.max.y > b.min.y) && (a.min.y < b.max.y); }
inline bool overlaps(const aabb2& a, const aabb2& b) { return overlaps_x(a, b) && overlaps_y(a, b); }
inline aabb2 include(const aabb2& a, vec2 p) { return aabb2(min(a.min, p), max(a.max, p)); }
inline aabb2 offset(const aabb2& a, vec2 offset_by) { return aabb2(a.min + offset_by, a.max + offset_by); }
inline aabb2 inflate(const aabb2& a, vec2 inflate_by) { return aabb2(a.min - inflate_by, a.max + inflate_by); }
inline aabb2 expand_toward(const aabb2& a, vec2 d) { return aabb2(vec2((d.x < 0.0f) ? a.min.x + d.x : a.min.x, (d.y < 0.0f) ? a.min.y + d.y : a.min.y), vec2((d.x > 0.0f) ? a.max.x + d.x : a.max.x, (d.y > 0.0f) ? a.max.y + d.y : a.max.y)); }

frustum make_frustum(const mat44& m);
vec3 intersect_planes_3(const vec4& a, const vec4& b, const vec4& c);

#endif // VEC_H