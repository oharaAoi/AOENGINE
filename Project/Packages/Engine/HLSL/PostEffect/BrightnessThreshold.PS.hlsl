#include "ProcessedScene.hlsli"

struct BloomSettings {
	float threshold;
};

ConstantBuffer<BloomSettings> gBloomSettings : register(b0);
Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct PixelShaderOutput {
	float4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input) {
	PixelShaderOutput output;
	float3 color = gTexture.Sample(gSampler, input.texcoord).rgb;

    // 輝度を求める（NTSC加重）
	float luminance = dot(color, float3(0.299, 0.587, 0.114));

    // 明るさしきい値でマスク
	float3 bright = (luminance > gBloomSettings.threshold) ? color : float3(0, 0, 0);

	output.color = float4(bright, 1.0);
	
	return output;
}