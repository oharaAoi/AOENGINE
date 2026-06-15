#include "ProcessedScene.hlsli"

struct BlurSetting {
	float2 blurCenter;	// 放射ブラーの中心
	float blurStrength; // ブラーの強度
	float blurStart;	// ブラーの開始距離
	int sampleCount;	// サンプル数
};

ConstantBuffer<BlurSetting> gBlurSetting : register(b0);
Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct PixelShaderOutput {
	float4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input) {
	PixelShaderOutput output;
	float2 dir = input.texcoord - gBlurSetting.blurCenter;
	float dist = length(dir);

	// ブラー強度を距離に応じて変化、閾値より内側ではゼロに
	float fade = saturate((dist - gBlurSetting.blurStart) / (1.0 - gBlurSetting.blurStart));
	float dynamicStrength = fade * gBlurSetting.blurStrength;

	float2 step = dir * dynamicStrength / gBlurSetting.sampleCount;

	float3 accum = float3(0, 0, 0);
	for (int i = 1; i <= gBlurSetting.sampleCount; ++i) {
		float2 offset = step * i;
		accum += gTexture.Sample(gSampler, clamp(input.texcoord - offset, 0.0, 1.0)).rgb;
		accum += gTexture.Sample(gSampler, clamp(input.texcoord + offset, 0.0, 1.0)).rgb;
	}

	float sampleTotal = gBlurSetting.sampleCount * 2;
	float3 blurred = accum / sampleTotal;

	output.color = float4(blurred, 1.0);
	return output;
}