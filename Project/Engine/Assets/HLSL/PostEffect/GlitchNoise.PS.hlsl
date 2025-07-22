#include "ProcessedScene.hlsli"
#include "../Random.hlsli"

struct GlitchNoiseSetting {
	float2 texelSize;
	float time;
	float glitchStrength;
	int frameIndex;
};

ConstantBuffer<GlitchNoiseSetting> gGlitchNoise : register(b0);
Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct PixelShaderOutput {
	float4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input) {
	PixelShaderOutput output;

	float2 uv = input.texcoord;
	PixelShaderOutput result;
	RandomGenerator rng;
	rng.seed = float3(uv * 500.0, gGlitchNoise.frameIndex * 17.234);

	float3 color = gTexture.Sample(gSampler, uv).rgb;

	if (gGlitchNoise.glitchStrength > 0.5) {
		// 横スキャンラインの歪み
		float scanlineChance = rng.Generated1d();
		if (scanlineChance > 0.2) {
			uv.x += rng.Generated1dRange(-0.4, 0.4);
			uv.y += sin(uv.x * 80.0 + gGlitchNoise.time * 15.0) * 0.02;
		}

		// stretc
		float stretchChance = rng.Generated1d();
		if (stretchChance > 0.9) {
			uv.y += sin(gGlitchNoise.time * 20.0) * rng.Generated1dRange(0.05, 0.1);
		}

		// 画面ブロック崩れ
		float2 blockSize = float2(rng.Generated1dRange(0.5f, 0.9f), rng.Generated1dRange(0.5f, 0.9f)); // 10x10 ブロック
		float2 blockId = floor(uv / blockSize);
		RandomGenerator blockRng;
		blockRng.seed = float3(blockId, gGlitchNoise.frameIndex * 11.31);
		float2 blockOffset = blockRng.Generated2dRangeSize(float2(-0.03, -0.03), float2(0.03, 0.03));
		uv += blockOffset;

		// RGBずれ
		float2 uvR = uv + float2(gGlitchNoise.texelSize.x * 10.0, 0.0);
		float2 uvB = uv - float2(gGlitchNoise.texelSize.x * 10.0, 0.0);
		float r = gTexture.Sample(gSampler, uvR).r;
		float g = gTexture.Sample(gSampler, uv).g;
		float b = gTexture.Sample(gSampler, uvB).b;

		color = float3(r, g, b);

		// ノイズフラッシュ
		float flash = rng.Generated1d();
		if (flash > 0.5) {
			color += float3(0.2, 0.2, 0.2);
		}

		float noise = rng.Generated1dRange(-0.05, 0.05);
		color += noise;
	}

	output.color = float4(color, 1.0);
	return output;
}
