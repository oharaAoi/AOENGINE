#include "ProcessedScene.hlsli"

struct BlurSettings {
	float2 texelSize;
};

ConstantBuffer<BlurSettings> gBlurSettings : register(b0);
Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct PixelShaderOutput {
	float4 color : SV_TARGET0;
};

// ガウス重み
static const float weights[5] = {
	0.227027f, // center
    0.1945946f,
    0.1216216f,
    0.054054f,
    0.016216f
};

PixelShaderOutput main(VertexShaderOutput input) {
	PixelShaderOutput output;
	float2 uv = input.texcoord;
	float3 color = gTexture.Sample(gSampler, uv).rgb * weights[0];

	for (int i = 1; i < 5; ++i) {
		float offset = float(i) * gBlurSettings.texelSize.x;
		color += gTexture.Sample(gSampler, uv + float2(offset, 0)).rgb * weights[i];
		color += gTexture.Sample(gSampler, uv - float2(offset, 0)).rgb * weights[i];
	}
	
	output.color = float4(color, 1.0);
	
	return output;
}