#include "ProcessedScene.hlsli"

struct BloomSettings {
	float bloomIntensity;
};

ConstantBuffer<BloomSettings> gBloomSettings : register(b0);
Texture2D<float4> gSceneTexture : register(t0);
Texture2D<float4> gBloomTexture : register(t1);
SamplerState gSampler : register(s0);

struct PixelShaderOutput {
	float4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input) {
	PixelShaderOutput output;
	float3 sceneColor = gSceneTexture.Sample(gSampler, input.texcoord).rgb;
	float3 bloomColor = gBloomTexture.Sample(gSampler, input.texcoord).rgb;

    // 加算合成（ブルーム強度スケールあり）
	float3 result = sceneColor + bloomColor * gBloomSettings.bloomIntensity;

	output.color = float4(result, 1.0); // HDRのまま返す
	return output;
}