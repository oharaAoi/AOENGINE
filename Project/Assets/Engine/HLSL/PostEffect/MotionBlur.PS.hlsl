#include "ProcessedScene.hlsli"

Texture2D<float4> gTexture : register(t0);
Texture2D<float2> gMotion : register(t1);
SamplerState gSampler : register(s0);

struct PixelShaderOutput {
	float4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input) {
	PixelShaderOutput output;
	output.color = gTexture.Sample(gSampler, input.texcoord);
	float2 mv = gMotion.Sample(gSampler, input.texcoord); // currentSS - prevSS
    
    // 片方向トレイル
	float2 dir = -mv * 0.3f; // UV空間

    // 小さいならそのまま
	if (dot(dir, dir) < 1e-6) {
		return output;
	}

	const int SAMPLES = 8;
	float3 accum = output.color.rgb;
	float wsum = 1.0f;

    [unroll]
	for (int j = 1; j < SAMPLES; ++j) {
		float t = (float) j / (SAMPLES - 1); // 0..1
		float2 uvS = input.texcoord + dir * t;
		float3 c = gTexture.Sample(gSampler, uvS).rgb;
		float w = 1.0f - t; // 線形フェード
		accum += c * w;
		wsum += w;
	}
	
	output.color = float4(accum / wsum, output.color.a);
	
	return output;
}