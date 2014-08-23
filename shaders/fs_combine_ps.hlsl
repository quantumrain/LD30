struct VS_OUTPUT {
	float4 position	: POSITION;
	float2 uv		: TEXCOORD0;
	float4 colour	: COLOR0;
};

sampler2D s0 : register(s0);
sampler2D s1 : register(s1);
sampler2D s2 : register(s2);
sampler2D s3 : register(s3);
sampler2D s4 : register(s4);
sampler2D s5 : register(s5);

float4 seed : register(c0);

float noise(float2 p) {
	float2 a = seed.xy + p;
	float b = frac(a.x / (3.0f + sin(a.x) + sin(a.y)));
	return b * b;
}

float4 main(VS_OUTPUT input) : SV_TARGET {
	float4 colour = tex2D(s0, input.uv);
	float4 bloom;

	bloom = tex2D(s1, input.uv);
	bloom += tex2D(s2, input.uv);
	bloom += tex2D(s3, input.uv);
	bloom += tex2D(s4, input.uv);
	bloom += tex2D(s5, input.uv);

	float4 r = colour + bloom * bloom * 0.1f;

	float4 noise_f = (1.0f - saturate(r)) * 0.1f;

	r *= (1.0f - noise_f) + (noise(input.uv * 1000.0f) * 2.0f) * noise_f;

	return r;
}