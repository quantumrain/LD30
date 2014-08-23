struct VS_OUTPUT {
	float4 position	: SV_POSITION;
	float2 uv		: TEXCOORD0;
	float4 colour	: COLOR0;
};

sampler2D s0 : register(s0);

float4 main(VS_OUTPUT input) : SV_TARGET {
	return tex2D(s0, input.uv);
}