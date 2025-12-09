SamplerState gSampler : register(s0);
Texture2D<float4> texA : register(t0);
Texture2D<float4> texB : register(t1);
RWTexture2D<float4> outputTex : register(u0);

[numthreads(16, 16, 1)]
void CSmain(uint3 id : SV_DispatchThreadID) {
	uint2 uv = id.xy;

	float4 a = texA[uv];
	float4 b = texB[uv];

	float mask = dot(b.rgb, float3(0.299, 0.587, 0.114));; // 0～1をマスク値にする

	float4 result = a * mask;

	outputTex[uv] = result;
}