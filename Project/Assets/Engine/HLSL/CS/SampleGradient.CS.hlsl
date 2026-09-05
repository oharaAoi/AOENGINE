struct GradientKey {
	float4 color;
	float position;
	float3 padding;
};

struct GradientParams {
	GradientKey keys[8];
	uint keyCount;
	uint channel;
	uint multiplySourceAlpha;
	uint padding;
};

ConstantBuffer<GradientParams> gGradient : register(b0);
Texture2D<float4> gInputTex : register(t0);
RWTexture2D<float4> gOutputTex : register(u0);

[numthreads(16, 16, 1)]
void CSmain(uint3 id : SV_DispatchThreadID) {
	uint width, height;
	gOutputTex.GetDimensions(width, height);
	if (id.x >= width || id.y >= height) { return; }
	float4 source = gInputTex.Load(int3(id.xy, 0));
	float value = gGradient.channel < 4 ? source[min(gGradient.channel, 3u)]
		: dot(source.rgb, float3(0.2126f, 0.7152f, 0.0722f));
	value = saturate(value);
	float4 color = gGradient.keys[0].color;
	for (uint i = 1; i < min(gGradient.keyCount, 8u); ++i) {
		float start = gGradient.keys[i - 1].position;
		float end = gGradient.keys[i].position;
		float t = end > start ? saturate((value - start) / (end - start)) : step(end, value);
		color = lerp(color, gGradient.keys[i].color, t);
	}
	if (gGradient.multiplySourceAlpha != 0) { color.a *= source.a; }
	gOutputTex[id.xy] = color;
}
