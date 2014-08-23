#ifndef RENDER_H

// ** GPU **

enum class gpu_format {
	RGBA
};

struct vertex_buffer;
struct shader;
struct texture;
struct depth_buffer;

vertex_buffer* CreateVertexBuffer(int size_in_bytes);
void DestroyVertexBuffer(vertex_buffer* vb);
void* Map(vertex_buffer* vb);
void Unmap(vertex_buffer* vb);

shader* CreateShader(const BYTE* vertexShader, int vertexShaderLength, const BYTE* pixelShader, int pixelShaderLength);
void DestroyShader(shader* decl);

texture* CreateTexture(int width, int height, gpu_format fmt, uint8_t* initial_data);
void DestroyTexture(texture* tex);
void SetSampler(int slot, bool tex_clamp, bool bilin);
void SetTexture(int slot, texture* tex);

depth_buffer* CreateDepth(int width, int height);
void DestroyDepth(depth_buffer* dss);

void GpuInit();
void GpuShutdown();

void Clear(uint32_t col);
void SetViewport(vec2i pos, vec2i size, vec2 depth);
void SetRenderTarget(texture* tex);
void SetDepthTarget(depth_buffer* dss);
void SetDefaultRenderTarget();
void SetPsConst(int slot, vec4 v);
void SetVsConst(int slot, vec4 v);
void SetVsConst(int slot, mat44 m);
void SetDepthMode(bool depth_compare);
void SetAlphaBlend(bool alpha_blend);
void SetVertexBuffer(vertex_buffer* vb);
void SetShader(shader* shdr);
void DrawVertices(int count, int base, bool as_lines);

// ** RENDER **

struct vertex {
	float x, y, z;
	float u, v;
	float r, g, b, a;
};

struct draw_buffer {
	draw_buffer();
	~draw_buffer();

	bool create(int max_vertices, int vertex_stride);
	void destroy();

	void clear();
	u8* alloc(int count);
	void flush();

	vertex_buffer* _vb;

	u8* _vertices;

	int _num_vertices;
	int _max_vertices;
	int _vertex_stride;
};

struct draw_call {
	texture* tex;
	int base;
	int count;

	draw_call(texture* tex_, int base_, int count_) : tex(tex_), base(base_), count(count_) { }
};

struct draw_list {
	draw_buffer verts;
	array<draw_call> calls;

	bool init(int max_tris);
	void destroy();
	void reset();

	vertex* alloc(texture* tex, int count);
	void render();
};

struct draw_context {
	draw_context(draw_list* dl_);
	~draw_context();

	void set_texture(texture *t);
	void set_colour(const colour& c);

	void push_texture(texture* t);
	void push_colour(const colour& c);
	void push_transform(const mat44& m);

	void pop_texture();
	void pop_colour();
	void pop_transform();

	void vert(float x, float y, float u, float v, float r, float g, float b, float a);
	void vert(vec2 p, vec2 uv, colour c);

	void line(vec2 p0, vec2 p1, float w, colour c0, colour c1);
	void line(vec2 p0, vec2 p1, float w, colour c);

	void tri(vec2 p0, vec2 p1, vec2 p2, colour c);
	void tri(vec2 p0, vec2 p1, vec2 p2, colour c0, colour c1, colour c2);
	void tri(vec2 p0, vec2 p1, vec2 p2, vec2 uv0, vec2 uv1, vec2 uv2, colour c0, colour c1, colour c2);

	void quad(vec2 p0, vec2 p1, vec2 p2, vec2 p3, colour c);
	void quad(vec2 p0, vec2 p1, vec2 p2, vec2 p3, colour c0, colour c1, colour c2, colour c3);
	void quad(vec2 p0, vec2 p1, vec2 p2, vec2 p3, vec2 uv0, vec2 uv1, vec2 uv2, vec2 uv3, colour c0, colour c1, colour c2, colour c3);

	void rect(vec2 p0, vec2 p1, colour c);
	void rect(vec2 p0, vec2 p1, colour c0, colour c1, colour c2, colour c3);
	void rect(vec2 p0, vec2 p1, vec2 uv0, vec2 uv1, colour c0, colour c1, colour c2, colour c3);

	draw_list* _dl;

	array<texture*> _texture;
	array<colour> _colour;
	array<mat44> _transform;

	NOCOPY(draw_context);
};

enum draw_tile_flags {
	DT_ROT_0 = 0,
	DT_ROT_90 = 1,
	DT_ROT_180 = 2,
	DT_ROT_270 = 3,
	DT_FLIP_X = 4,
	DT_FLIP_Y = 8,
	DT_ALT_TRI = 16
};

void draw_tile(draw_context* dc, vec2 p0, vec2 p1, vec2 p2, vec2 p3, colour c0, colour c1, colour c2, colour c3, int tile_num, int flags);
void draw_tile(draw_context* dc, vec2 c, float s, float rot, colour col, int tile_num, int flags);
void draw_tile(draw_context* dc, vec2 c, float s, colour col, int tile_num, int flags);
void draw_tile(draw_context* dc, vec2 p0, vec2 p1, colour col, int tile_num, int flags);
void draw_tile(draw_context* dc, vec2 p0, vec2 p1, colour c0, colour c1, colour c2, colour c3, int tile_num, int flags);

enum font_flags {
	TEXT_LEFT	= 1,
	TEXT_RIGHT	= 2,
	TEXT_CENTRE	= 3,
	TEXT_TOP	= 4,
	TEXT_BOTTOM	= 8,
	TEXT_VCENTRE= 12
};

float measure_char(int c);
float measure_string(const char* txt);
void draw_char(draw_context* dc, vec2 pos, vec2 scale, int sprite, colour col);
void draw_string(draw_context* dc, vec2 pos, vec2 scale, int flags, colour col, const char* txt, ...);

extern draw_list g_dl_world;
extern draw_list g_dl_ui;

extern texture* g_sheet;

void set_proj_view(mat44 proj_view);

void RenderInit();
void RenderShutdown();

#endif // RENDER_H