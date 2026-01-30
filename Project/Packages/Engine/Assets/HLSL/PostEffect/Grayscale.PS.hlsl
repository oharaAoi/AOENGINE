#include "ProcessedScene.hlsli"

struct Setting {
	float4 color;
};

ConstantBuffer<Setting> gSetting : register(b0);
Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct PixelShaderOutput {
	float4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input) {
	PixelShaderOutput output;
	output.color = gTexture.Sample(gSampler, input.texcoord);
	
	float3 blendedColor = lerp(output.color.rgb, gSetting.color.rgb, gSetting.color.a);
	output.color.rgb *= blendedColor;
	output.color.a = 1.0f;
	
	return output;
}