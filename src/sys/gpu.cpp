#include "Pch.h"
#include "Common.h"

extern IDirect3DDevice9* gDevice;

// init

IDirect3DSurface9* g_default_render_target;

void GpuInit() {
	gDevice->GetRenderTarget(0, &g_default_render_target);
}

void GpuShutdown() {
	if (g_default_render_target) {
		g_default_render_target->Release();
		g_default_render_target = 0;
	}
}

// vertex_buffer

struct vertex_buffer {
	IDirect3DVertexBuffer9* vb;
};

vertex_buffer* CreateVertexBuffer(int size_in_bytes) {
	vertex_buffer* vb = new vertex_buffer;

	if (!vb)
		return 0;

	if (FAILED(gDevice->CreateVertexBuffer(size_in_bytes, D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, 0, D3DPOOL_DEFAULT, &vb->vb, 0))) {
		delete vb;
		return 0;
	}

	return vb;
}

void DestroyVertexBuffer(vertex_buffer* vb) {
	if (vb) {
		vb->vb->Release();
		delete vb;
	}
}

void* Map(vertex_buffer* vb) {
	if (vb) {
		void* data = 0;

		if (SUCCEEDED(vb->vb->Lock(0, 0, &data, D3DLOCK_DISCARD)))
			return data;
	}

	return 0;
}

void Unmap(vertex_buffer* vb) {
	if (vb)
		vb->vb->Unlock();
}

// shader

struct shader {
	IDirect3DVertexShader9* vertex;
	IDirect3DPixelShader9* pixel;
	IDirect3DVertexDeclaration9* decl;
};

shader* CreateShader(const BYTE* vertexShader, int vertexShaderLength, const BYTE* pixelShader, int pixelShaderLength) {
	shader* decl = new shader;

	if (!decl)
		return 0;

	if (FAILED(gDevice->CreateVertexShader((DWORD*)vertexShader, &decl->vertex))) {
		panic("CreateVertexShader failed");
	}

	if (FAILED(gDevice->CreatePixelShader((DWORD*)pixelShader, &decl->pixel))) {
		panic("CreatePixelShader failed");
	}

	D3DVERTEXELEMENT9 ve[4] =
	{
		{ 0, offsetof(vertex, x), D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0, offsetof(vertex, u), D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		{ 0, offsetof(vertex, r), D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
		D3DDECL_END()
	};

	if (FAILED(gDevice->CreateVertexDeclaration(ve, &decl->decl))) {
		panic("CreateInputLayout failed");
	}

	return decl;
}

void DestroyShader(shader* decl) {
	if (decl) {
		decl->vertex->Release();
		decl->pixel->Release();
		decl->decl->Release();

		delete decl;
	}
}

// texture

struct texture {
	IDirect3DTexture9* tex;
};

texture* CreateTexture(int width, int height, gpu_format fmt, uint8_t* initial_data) {
	texture* tex = new texture;

	if (!tex)
		return 0;

	D3DFORMAT dfmt = D3DFMT_A8R8G8B8;

	if (FAILED(gDevice->CreateTexture(width, height, 1, initial_data ? 0 : D3DUSAGE_RENDERTARGET, dfmt, initial_data ? D3DPOOL_MANAGED : D3DPOOL_DEFAULT, &tex->tex, 0))) {
		delete tex;
		return 0;
	}

	if (initial_data) {
		D3DLOCKED_RECT lr;

		if (SUCCEEDED(tex->tex->LockRect(0, &lr, 0, D3DLOCK_DISCARD))) {
			for(int y = 0; y < height; y++) {
				//memcpy((uint8_t*)lr.pBits + y * lr.Pitch, initial_data + y * width * 4, width * 4);
				uint8_t* d = (uint8_t*)lr.pBits + y * lr.Pitch;
				uint8_t* s = initial_data + y * width * 4;

				for(int x = 0; x < width; x++, d += 4, s+= 4) {
					d[0] = s[2];
					d[1] = s[1];
					d[2] = s[0];
					d[3] = s[3];
				}
			}

			tex->tex->UnlockRect(0);
		}
	}

	return tex;
}

void DestroyTexture(texture* tex) {
	if (tex) {
		tex->tex->Release();
		delete tex;
	}
}

void SetSampler(int slot, bool tex_clamp, bool bilin) {
	gDevice->SetSamplerState(slot, D3DSAMP_ADDRESSU, tex_clamp ? D3DTADDRESS_CLAMP : D3DTADDRESS_WRAP);
	gDevice->SetSamplerState(slot, D3DSAMP_ADDRESSV, tex_clamp ? D3DTADDRESS_CLAMP : D3DTADDRESS_WRAP);

	gDevice->SetSamplerState(slot, D3DSAMP_MINFILTER, bilin ? D3DTEXF_LINEAR : D3DTEXF_POINT);
	gDevice->SetSamplerState(slot, D3DSAMP_MAGFILTER, bilin ? D3DTEXF_LINEAR : D3DTEXF_POINT);
	gDevice->SetSamplerState(slot, D3DSAMP_MIPFILTER, bilin ? D3DTEXF_LINEAR : D3DTEXF_POINT);

	gDevice->SetSamplerState(slot, D3DSAMP_SRGBTEXTURE, TRUE);
}

void SetTexture(int slot, texture* tex) {
	gDevice->SetTexture(slot, tex ? tex->tex : 0);
}

// depth_buffer

struct depth_buffer {
	IDirect3DSurface9* dss;
};

depth_buffer* CreateDepth(int width, int height) {
	depth_buffer* dss = new depth_buffer;

	if (!dss)
		return 0;

	if (FAILED(gDevice->CreateDepthStencilSurface(width, height, D3DFMT_D24S8, D3DMULTISAMPLE_NONE, 0, TRUE, &dss->dss, 0))) {
		delete dss;
		return 0;
	}

	return dss;
}

void DestroyDepth(depth_buffer* dss) {
	if (dss) {
		dss->dss->Release();
		delete dss;
	}
}

// control

void Clear(uint32_t col) {
	gDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, col, 1.0f, 0);
}

void SetRenderTarget(texture* tex) {
	IDirect3DSurface9* surf = 0;

	if (tex && FAILED(tex->tex->GetSurfaceLevel(0, &surf)))
		surf = 0;

	gDevice->SetRenderTarget(0, surf);

	if (surf)
		surf->Release();
}

void SetDepthTarget(depth_buffer* dss) {
	gDevice->SetDepthStencilSurface(dss ? dss->dss : 0);
}

void SetDefaultRenderTarget() {
	gDevice->SetRenderTarget(0, g_default_render_target);
}

void SetViewport(vec2i pos, vec2i size, vec2 depth) {
	D3DVIEWPORT9 vp;

	vp.Width = size.x;
	vp.Height = size.y;
	vp.MinZ = depth.x;
	vp.MaxZ = depth.y;
	vp.X = pos.x;
	vp.Y = pos.y;

	gDevice->SetViewport(&vp);
}

void SetVsConst(int slot, vec4 v)	{ gDevice->SetVertexShaderConstantF(slot, (float*)&v, 1); }
void SetVsConst(int slot, mat44 m)	{ gDevice->SetVertexShaderConstantF(slot, (float*)&m, 4); }

void SetPsConst(int slot, vec4 v)	{ gDevice->SetPixelShaderConstantF(slot, (float*)&v, 1); }

void SetDepthMode(bool depth_compare) {
	if (depth_compare) {
		gDevice->SetRenderState(D3DRS_ZENABLE,		D3DZB_TRUE);
		gDevice->SetRenderState(D3DRS_ZWRITEENABLE,	TRUE);
		gDevice->SetRenderState(D3DRS_ZFUNC,		D3DCMP_LESSEQUAL);
	} else {
		gDevice->SetRenderState(D3DRS_ZENABLE,		D3DZB_FALSE);
		gDevice->SetRenderState(D3DRS_ZWRITEENABLE,	FALSE);
	}
}

void SetAlphaBlend(bool alpha_blend) {
	if (alpha_blend) {
		gDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		gDevice->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, FALSE);
		gDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
		gDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
		gDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	} else {
		gDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	}

	gDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	gDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, TRUE);
}

void SetVertexBuffer(vertex_buffer* vb) {
	gDevice->SetStreamSource(0, vb ? vb->vb : 0, 0, sizeof(vertex));
}

void SetShader(shader* shdr) {
	gDevice->SetVertexDeclaration(shdr ? shdr->decl : 0);
	gDevice->SetVertexShader(shdr ? shdr->vertex : 0);
	gDevice->SetPixelShader(shdr ? shdr->pixel : 0);
}

void DrawVertices(int count, int base, bool as_lines) {
	if (as_lines)
		gDevice->DrawPrimitive(D3DPT_LINESTRIP, base, count - 1);
	else
		gDevice->DrawPrimitive(D3DPT_TRIANGLELIST, base, count / 3);
}