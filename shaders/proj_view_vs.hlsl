struct VS_INPUT {
	float3 position	: POSITION;
	float2 uv		: TEXCOORD0;
	float4 colour	: COLOR0;
};

struct VS_OUTPUT {
	float4 position	: SV_POSITION;
	float2 uv		: TEXCOORD0;
	float4 colour	: COLOR0;
};

float4x4 proj_view : register(c0);

VS_OUTPUT main(VS_INPUT v) {
	VS_OUTPUT output;

	output.position	= mul(proj_view, float4(v.position, 1.0f));
	output.uv		= v.uv;
	output.colour	= v.colour;

	return output;
}