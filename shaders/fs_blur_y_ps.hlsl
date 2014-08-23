struct VS_OUTPUT {
	float4 position	: SV_POSITION;
	float2 uv		: TEXCOORD0;
	float4 colour	: COLOR0;
};

float2 texel_size : register(c0);

sampler2D s0 : register(s0);

float4 main(VS_OUTPUT input) : SV_TARGET {
	float4 r;

	r  = tex2D(s0, float2(input.uv.x, input.uv.y					  )) * 0.319f;
	r += tex2D(s0, float2(input.uv.x, input.uv.y - texel_size.y		  )) * 0.232f;
	r += tex2D(s0, float2(input.uv.x, input.uv.y + texel_size.y		  )) * 0.232f;
	r += tex2D(s0, float2(input.uv.x, input.uv.y - 2.0f * texel_size.y)) * 0.089f;
	r += tex2D(s0, float2(input.uv.x, input.uv.y + 2.0f * texel_size.y)) * 0.089f;
	r += tex2D(s0, float2(input.uv.x, input.uv.y - 3.0f * texel_size.y)) * 0.018f;
	r += tex2D(s0, float2(input.uv.x, input.uv.y + 3.0f * texel_size.y)) * 0.018f;
	r += tex2D(s0, float2(input.uv.x, input.uv.y - 4.0f * texel_size.y)) * 0.002f;
	r += tex2D(s0, float2(input.uv.x, input.uv.y + 4.0f * texel_size.y)) * 0.002f;

	return r;
}