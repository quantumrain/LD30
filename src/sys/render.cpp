#include "Pch.h"
#include "Common.h"
#include "fs_blur_x_ps.h"
#include "fs_blur_y_ps.h"
#include "fs_combine_ps.h"
#include "fs_reduce_ps.h"
#include "proj_view_vs.h"
#include "tex_ps.h"

random g_render_rand(1);

vertex_buffer* gFsQuadVb;

shader* gBlurXDecl;
shader* gBlurYDecl;
shader* gCombineDecl;
shader* gReduceDecl;
shader* gTexDecl;

mat44 g_proj_view;

void set_proj_view(mat44 proj_view) { g_proj_view = proj_view; }

draw_list g_dl_world;
draw_list g_dl_ui;

struct bloom_level {
	vec2i size;
	texture* reduce;
	texture* blur_x;
	texture* blur_y;

	bloom_level() : reduce(), blur_x(), blur_y() { }
	~bloom_level() { destroy(); }

	void destroy() {
		DestroyTexture(reduce);
		DestroyTexture(blur_x);
		DestroyTexture(blur_y);

		reduce = 0;
		blur_x = 0;
		blur_y = 0;
	}
};

const int MAX_BLOOM_LEVELS = 5;
bloom_level g_bloom_levels[MAX_BLOOM_LEVELS];
texture* g_draw_target;
depth_buffer* g_depth_target;
texture* g_font;
texture* g_sheet;

void RenderInit() {
	gBlurXDecl		= CreateShader(g_proj_view_vs, sizeof(g_proj_view_vs), g_fs_blur_x_ps, sizeof(g_fs_blur_x_ps));
	gBlurYDecl		= CreateShader(g_proj_view_vs, sizeof(g_proj_view_vs), g_fs_blur_y_ps, sizeof(g_fs_blur_y_ps));
	gCombineDecl	= CreateShader(g_proj_view_vs, sizeof(g_proj_view_vs), g_fs_combine_ps, sizeof(g_fs_combine_ps));
	gReduceDecl		= CreateShader(g_proj_view_vs, sizeof(g_proj_view_vs), g_fs_reduce_ps, sizeof(g_fs_reduce_ps));
	gTexDecl		= CreateShader(g_proj_view_vs, sizeof(g_proj_view_vs), g_tex_ps, sizeof(g_tex_ps));

	g_dl_world.init(256 * 1024);
	g_dl_ui.init(256 * 1024);

	gFsQuadVb = CreateVertexBuffer(sizeof(vertex) * 3);

	vec2i size(g_view_size);

	g_draw_target = CreateTexture(size.x, size.y, gpu_format::RGBA, 0);
	g_depth_target = CreateDepth(size.x, size.y);

	for(int i = 0; i < MAX_BLOOM_LEVELS; i++) {
		bloom_level* bl = g_bloom_levels + i;

		size /= 2;

		bl->size = size;
		bl->reduce = CreateTexture(size.x, size.y, gpu_format::RGBA, 0);
		bl->blur_x = CreateTexture(size.x, size.y, gpu_format::RGBA, 0);
		bl->blur_y = CreateTexture(size.x, size.y, gpu_format::RGBA, 0);
	}

	g_font = load_texture("data\\font.png");
	g_sheet = load_texture("data\\sheet.png");
}

void RenderShutdown() {
	DestroyShader(gBlurXDecl);
	DestroyShader(gBlurYDecl);
	DestroyShader(gCombineDecl);
	DestroyShader(gReduceDecl);
	DestroyShader(gTexDecl);

	g_dl_world.destroy();
	g_dl_ui.destroy();

	DestroyVertexBuffer(gFsQuadVb);

	DestroyTexture(g_draw_target);
	DestroyDepth(g_depth_target);

	for(int i = 0; i < MAX_BLOOM_LEVELS; i++)
		g_bloom_levels[i].destroy();

	DestroyTexture(g_font);
	DestroyTexture(g_sheet);
}

void RenderPreUpdate() {
	g_dl_world.reset();
	g_dl_ui.reset();
}

void do_fullscreen_quad(shader* decl, vec2 size) {
	if (vertex* v = (vertex*)Map(gFsQuadVb)) {
		float ax = -(1.0f / size.x);
		float ay = (1.0f / size.y);

		v->x = -1.0f + ax;
		v->y = -1.0f + ay;
		v->u = 0.0f;
		v->v = 1.0f;
		v->r = v->g = v->b = v->a = 1.0f;
		v++;

		v->x = -1.0f + ax;
		v->y = 3.0f + ay;
		v->u = 0.0f;
		v->v = -1.0f;
		v->r = v->g = v->b = v->a = 1.0f;
		v++;

		v->x = 3.0f + ax;
		v->y = -1.0f + ay;
		v->u = 2.0f;
		v->v = 1.0f;
		v->r = v->g = v->b = v->a = 1.0f;

		Unmap(gFsQuadVb);
	}

	SetVsConst(0, mat44());
	SetShader(decl);
	SetVertexBuffer(gFsQuadVb);
	DrawVertices(3, 0, false);
}

void frame_render() {
	SetRenderTarget(g_draw_target);
	SetDepthTarget(g_depth_target);

	SetDepthMode(false);
	SetAlphaBlend(true);

	SetViewport(vec2i(0, 0), g_view_size, vec2(0.0f, 1.0f));
	Clear(0x00000000);

	SetShader(gTexDecl);
	SetVsConst(0, g_proj_view);

	g_dl_world.render();

	float height	= 360.0f;
	float width		= height * g_view_aspect;
	float left		= (640.0f - width) * 0.5f;
	float top		= (360.0f - height) * 0.5f;

	SetVsConst(0, ortho(left, left + width, top + height, top, -64.0f, 64.0f));

	g_dl_ui.render();

	// pp

	SetAlphaBlend(false);
	SetDepthTarget(0);

	for(int i = 0; i < MAX_BLOOM_LEVELS; i++) {
		bloom_level* bl = g_bloom_levels + i;

		// reduce
		SetRenderTarget(bl->reduce);
		SetViewport(vec2i(), bl->size, vec2(0.0f, 1.0f));
		SetTexture(0, (i == 0) ? g_draw_target : g_bloom_levels[i - 1].blur_y);
		SetSampler(0, true, true);
		SetPsConst(0, vec4(1.0f / to_vec2(bl->size), 0.0f, 0.0f));
		do_fullscreen_quad(gReduceDecl, to_vec2(bl->size));

		// blur x
		SetRenderTarget(bl->blur_x);
		SetViewport(vec2i(), bl->size, vec2(0.0f, 1.0f));
		SetTexture(0, bl->reduce);
		SetSampler(0, true, true);
		SetPsConst(0, vec4(1.0f / to_vec2(bl->size), 0.0f, 0.0f));
		do_fullscreen_quad(gBlurXDecl, to_vec2(bl->size));

		// blur y
		SetRenderTarget(bl->blur_y);
		SetViewport(vec2i(), bl->size, vec2(0.0f, 1.0f));
		SetTexture(0, bl->blur_x);
		SetSampler(0, true, true);
		SetPsConst(0, vec4(1.0f / to_vec2(bl->size), 0.0f, 0.0f));
		do_fullscreen_quad(gBlurYDecl, to_vec2(bl->size));
	}

	// combine
	SetDefaultRenderTarget();
	SetViewport(vec2i(0, 0), g_view_size, vec2(0.0f, 1.0f));
	SetPsConst(0, g_render_rand.range(vec4(500.0f), vec4(1500.0f)));
	SetTexture(0, g_draw_target);
	SetSampler(0, true, false);
	for(int i = 0; i < MAX_BLOOM_LEVELS; i++) {
		SetTexture(1 + i, g_bloom_levels[i].blur_y);
		SetSampler(1 + i, true, true);
	}
	do_fullscreen_quad(gCombineDecl, to_vec2(g_view_size));
}