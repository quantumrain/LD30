#include "Pch.h"
#include "Common.h"

// draw_buffer

draw_buffer::draw_buffer() : _vb(0), _vertices(0), _num_vertices(0), _max_vertices(0), _vertex_stride(0) { }

draw_buffer::~draw_buffer() {
	destroy();
}

bool draw_buffer::create(int max_vertices, int vertex_stride) {
	destroy();

	if ((max_vertices <= 0) || (vertex_stride <= 0))
		return false;

	_num_vertices = 0;
	_max_vertices = max_vertices;
	_vertex_stride = vertex_stride;

	int size_in_bytes = max_vertices * vertex_stride;

	if (!(_vertices = new u8[size_in_bytes]) || !(_vb = CreateVertexBuffer(size_in_bytes))) {
		destroy();
		return false;
	}

	return true;
}

void draw_buffer::destroy() {
	DestroyVertexBuffer(_vb);

	delete [] _vertices;

	_vb = 0;
	_vertices = 0;
	_num_vertices = 0;
	_max_vertices = 0;
	_vertex_stride = 0;
}

void draw_buffer::clear() {
	_num_vertices = 0;
}

uint8_t* draw_buffer::alloc(int count) {
	if ((_num_vertices + count) > _max_vertices)
		return 0;

	uint8_t* p = _vertices + _num_vertices * _vertex_stride;

	_num_vertices += count;

	return p;
}

void draw_buffer::flush() {
	if (_num_vertices <= 0)
		return;

	if (void* p = Map(_vb)) {
		memcpy(p, _vertices, _num_vertices * _vertex_stride);
		Unmap(_vb);
	}
}

// draw_list

bool draw_list::init(int max_tris) {
	return verts.create(max_tris * 3, sizeof(vertex));
}

void draw_list::destroy() {
	verts.destroy();
}

void draw_list::reset() {
	verts.clear();
	calls.clear();
}

vertex* draw_list::alloc(texture* tex, int count) {
	draw_call* call = calls.not_empty() ? &calls.back() : 0;

	if (!call || (call->tex != tex)) {
		if ((call = calls.push_back(draw_call(tex, verts._num_vertices, 0))) == 0)
			return 0;
	}

	vertex* v = (vertex*)verts.alloc(count);

	if (v)
		call->count += count;

	return v;
}

void draw_list::render() {
	if (calls.empty())
		return;

	verts.flush();

	SetVertexBuffer(verts._vb);
	SetSampler(0, false, false);

	for(auto& c : calls) {
		SetTexture(0, c.tex);
		DrawVertices(c.count, c.base, false);
	}
}

// draw_context

const float WHITE_UV = 248.0f / 256.0f;

draw_context::draw_context(draw_list* dl) : _dl(dl) {
	_texture.reserve(16);
	_colour.reserve(16);
	_transform.reserve(16);

	_texture.push_back(0);
	_colour.push_back(colour());
	_transform.push_back(mat44());
}

draw_context::~draw_context() {
	assert(_texture.size() == 1);
	assert(_colour.size() == 1);
	assert(_transform.size() == 1);
}

void draw_context::set_texture(texture *t) {
	_texture.back() = t;
}

void draw_context::set_colour(const colour& c) {
	_colour.back() = c;
}

void draw_context::set_transform(const mat44& m) {
	if (_transform.size() > 1)
		_transform.back() = _transform[_transform.size() - 2] * m;
	else
		_transform.back() = m;
}

void draw_context::push_texture(texture* t) {
	_texture.push_back(t);
}

void draw_context::push_colour(const colour& c) {
	_colour.push_back(_colour.back() * c);
}

void draw_context::push_transform(const mat44& m) {
	_transform.push_back(_transform.back() * m);
}

void draw_context::pop_texture() {
	_texture.pop_back();
}

void draw_context::pop_colour() {
	_colour.pop_back();
}

void draw_context::pop_transform() {
	_transform.pop_back();
}

void draw_context::vert(float x, float y, float u, float v, float r, float g, float b, float a) {
	vertex* vtx = _dl->alloc(_texture.back(), 1);

	if (!vtx)
		return;

	colour c = _colour.back();
	vec4 p = _transform.back() * vec4(x, y, 0.0f, 1.0f);

	vtx->x = p.x;
	vtx->y = p.y;
	vtx->z = p.z;
	vtx->u = u;
	vtx->v = v;
	vtx->r = square(r * c.r);
	vtx->g = square(g * c.g);
	vtx->b = square(b * c.b);
	vtx->a = a * c.a;
}

void draw_context::vert(vec2 p, vec2 uv, colour c) {
	vert(p.x, p.y, uv.x, uv.y, c.r, c.g, c.b, c.a);
}

void draw_context::line(vec2 p0, vec2 p1, float w, colour c0, colour c1) {
	vec2 n = perp(normalise(p1 - p0)) * w;

	vec2 a = p0 - n;
	vec2 b = p0 + n;
	vec2 c = p1 - n;
	vec2 d = p1 + n;

	tri(a, c, b, c0, c1, c0);
	tri(b, c, d, c0, c1, c1);
}

void draw_context::line(vec2 p0, vec2 p1, float w, colour c) {
	line(p0, p1, w, c, c);
}

void draw_context::tri(vec2 p0, vec2 p1, vec2 p2, colour c) {
	vert(p0, vec2(WHITE_UV), c);
	vert(p1, vec2(WHITE_UV), c);
	vert(p2, vec2(WHITE_UV), c);
}

void draw_context::tri(vec2 p0, vec2 p1, vec2 p2, colour c0, colour c1, colour c2) {
	vert(p0, vec2(WHITE_UV), c0);
	vert(p1, vec2(WHITE_UV), c1);
	vert(p2, vec2(WHITE_UV), c2);
}

void draw_context::tri(vec2 p0, vec2 p1, vec2 p2, vec2 uv0, vec2 uv1, vec2 uv2, colour c0, colour c1, colour c2) {
	vert(p0, uv0, c0);
	vert(p1, uv1, c1);
	vert(p2, uv2, c2);
}

void draw_context::quad(vec2 p0, vec2 p1, vec2 p2, vec2 p3, colour c) {
	vert(p0, vec2(WHITE_UV), c);
	vert(p2, vec2(WHITE_UV), c);
	vert(p1, vec2(WHITE_UV), c);

	vert(p1, vec2(WHITE_UV), c);
	vert(p2, vec2(WHITE_UV), c);
	vert(p3, vec2(WHITE_UV), c);
}

void draw_context::quad(vec2 p0, vec2 p1, vec2 p2, vec2 p3, colour c0, colour c1, colour c2, colour c3) {
	vert(p0, vec2(WHITE_UV), c0);
	vert(p2, vec2(WHITE_UV), c2);
	vert(p1, vec2(WHITE_UV), c1);

	vert(p1, vec2(WHITE_UV), c1);
	vert(p2, vec2(WHITE_UV), c2);
	vert(p3, vec2(WHITE_UV), c3);
}

void draw_context::quad(vec2 p0, vec2 p1, vec2 p2, vec2 p3, vec2 uv0, vec2 uv1, vec2 uv2, vec2 uv3, colour c0, colour c1, colour c2, colour c3) {
	vert(p0, uv0, c0);
	vert(p2, uv2, c2);
	vert(p1, uv1, c1);

	vert(p1, uv1, c1);
	vert(p2, uv2, c2);
	vert(p3, uv3, c3);
}

void draw_context::rect(vec2 p0, vec2 p1, colour c) {
	vert(p0.x, p0.y, WHITE_UV, WHITE_UV, c.r, c.g, c.b, c.a);
	vert(p0.x, p1.y, WHITE_UV, WHITE_UV, c.r, c.g, c.b, c.a);
	vert(p1.x, p0.y, WHITE_UV, WHITE_UV, c.r, c.g, c.b, c.a);

	vert(p1.x, p0.y, WHITE_UV, WHITE_UV, c.r, c.g, c.b, c.a);
	vert(p0.x, p1.y, WHITE_UV, WHITE_UV, c.r, c.g, c.b, c.a);
	vert(p1.x, p1.y, WHITE_UV, WHITE_UV, c.r, c.g, c.b, c.a);
}

void draw_context::rect(vec2 p0, vec2 p1, colour c0, colour c1, colour c2, colour c3) {
	vert(p0.x, p0.y, WHITE_UV, WHITE_UV, c0.r, c0.g, c0.b, c0.a);
	vert(p0.x, p1.y, WHITE_UV, WHITE_UV, c2.r, c2.g, c2.b, c2.a);
	vert(p1.x, p0.y, WHITE_UV, WHITE_UV, c1.r, c1.g, c1.b, c1.a);

	vert(p1.x, p0.y, WHITE_UV, WHITE_UV, c1.r, c1.g, c1.b, c1.a);
	vert(p0.x, p1.y, WHITE_UV, WHITE_UV, c2.r, c2.g, c2.b, c2.a);
	vert(p1.x, p1.y, WHITE_UV, WHITE_UV, c3.r, c3.g, c3.b, c3.a);
}

void draw_context::rect(vec2 p0, vec2 p1, vec2 uv0, vec2 uv1, colour c0, colour c1, colour c2, colour c3) {
	vert(p0.x, p0.y, uv0.x, uv0.y, c0.r, c0.g, c0.b, c0.a);
	vert(p0.x, p1.y, uv0.x, uv1.y, c2.r, c2.g, c2.b, c2.a);
	vert(p1.x, p0.y, uv1.x, uv0.y, c1.r, c1.g, c1.b, c1.a);

	vert(p1.x, p0.y, uv1.x, uv0.y, c1.r, c1.g, c1.b, c1.a);
	vert(p0.x, p1.y, uv0.x, uv1.y, c2.r, c2.g, c2.b, c2.a);
	vert(p1.x, p1.y, uv1.x, uv1.y, c3.r, c3.g, c3.b, c3.a);
}

// helpers

void draw_tile(draw_context* dc, vec2 p0, vec2 p1, vec2 p2, vec2 p3, colour c0, colour c1, colour c2, colour c3, int tile_num, int flags) {
	int tx = tile_num & 15;
	int ty = tile_num >> 4;

	vec2 uv0;
	vec2 uv1;
	vec2 uv2;
	vec2 uv3;

	const float a = 0.00001f / 16.0f; // UUURGH
	const float b = 15.99999f / 16.0f;

	if ((flags & 1) == 0) {
		uv0 = vec2((tx + a) / 16.0f, (ty + a) / 16.0f);
		uv1 = vec2((tx + b) / 16.0f, (ty + a) / 16.0f);
		uv2 = vec2((tx + a) / 16.0f, (ty + b) / 16.0f);
		uv3 = vec2((tx + b) / 16.0f, (ty + b) / 16.0f);
	} else {
		uv0 = vec2((tx + a) / 16.0f, (ty + b) / 16.0f);
		uv1 = vec2((tx + a) / 16.0f, (ty + a) / 16.0f);
		uv2 = vec2((tx + b) / 16.0f, (ty + b) / 16.0f);
		uv3 = vec2((tx + b) / 16.0f, (ty + a) / 16.0f);
	}

	if (flags & 2) {
		swap(uv0, uv2);
		swap(uv1, uv3);
		swap(uv0, uv1);
		swap(uv2, uv3);
	}

	if (flags & DT_FLIP_X) {
		swap(uv0, uv1);
		swap(uv2, uv3);
	}

	if (flags & DT_FLIP_Y) {
		swap(uv0, uv2);
		swap(uv1, uv3);
	}

	if (flags & DT_ALT_TRI) {
		dc->vert(p0, uv0, c0);
		dc->vert(p3, uv3, c3);
		dc->vert(p1, uv1, c1);

		dc->vert(p0, uv0, c0);
		dc->vert(p2, uv2, c2);
		dc->vert(p3, uv3, c3);
	}
	else {
		dc->vert(p0, uv0, c0);
		dc->vert(p2, uv2, c2);
		dc->vert(p1, uv1, c1);

		dc->vert(p1, uv1, c1);
		dc->vert(p2, uv2, c2);
		dc->vert(p3, uv3, c3);
	}
}

void draw_tile(draw_context* dc, vec2 c, float s, float rot, colour col, int tile_num, int flags) {
	vec2 cx(cosf(rot) * s * 0.5f, sinf(rot) * s * 0.5f);
	vec2 cy(perp(cx));
	draw_tile(dc, c - cx - cy, c + cx - cy, c - cx + cy, c + cx + cy, col, col, col, col, tile_num, flags);
}

void draw_tile(draw_context* dc, vec2 c, float s, colour col, int tile_num, int flags) {
	vec2 cx(s, 0.0f);
	vec2 cy(0.0f, s);
	draw_tile(dc, c - cx - cy, c + cx - cy, c - cx + cy, c + cx + cy, col, col, col, col, tile_num, flags);
}

void draw_tile(draw_context* dc, vec2 p0, vec2 p1, colour col, int tile_num, int flags) {
	draw_tile(dc, p0, vec2(p1.x, p0.y), vec2(p0.x, p1.y), p1, col, col, col, col, tile_num, flags);
}

void draw_tile(draw_context* dc, vec2 p0, vec2 p1, colour c0, colour c1, colour c2, colour c3, int tile_num, int flags) {
	draw_tile(dc, p0, vec2(p1.x, p0.y), vec2(p0.x, p1.y), p1, c0, c1, c2, c3, tile_num, flags);
}