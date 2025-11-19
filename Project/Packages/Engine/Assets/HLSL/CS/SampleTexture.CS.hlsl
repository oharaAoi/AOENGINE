
struct UVParam {
	float2 scale;
	float2 translate;
	float rotate;
};

Texture2D<float4> gInputTex : register(t0);
ConstantBuffer<UVParam> gUV : register(b0);
RWTexture2D<float4> gPreviewTex : register(u0);
SamplerState gSampler : register(s0);

[numthreads(16, 16, 1)]
void CSmain(uint3 id : SV_DispatchThreadID) {
	uint2 pix = id.xy;

	uint w, h;
	gPreviewTex.GetDimensions(w, h);

	float2 uv = (pix + 0.5) / float2(w, h);

	float c = cos(gUV.rotate);
	float s = sin(gUV.rotate);

	uv *= gUV.scale;

	uv = float2(
    uv.x * c - uv.y * s,
    uv.x * s + uv.y * c
	);

	uv += gUV.translate;

	float4 col = gInputTex.SampleLevel(gSampler, uv, 0);

	gPreviewTex[pix] = col;
}