#include "ProcessedScene.hlsli"

struct Dissolve {
	float4x4 uvTransform;
	float4 color;
	float4 edgeColor;
	float threshold;
};

ConstantBuffer<Dissolve> gSetting : register(b0);
Texture2D<float4> gTexture : register(t0);
Texture2D<float> gMaskTexture : register(t1);
SamplerState gSampler : register(s0);

struct PixelShaderOutput {
	float4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input) {
	PixelShaderOutput output;
	float4 transformedUV = mul(float4(input.texcoord, 0.0f, 1.0f), gSetting.uvTransform);
	// maskを計算する
	float mask = gMaskTexture.Sample(gSampler, transformedUV.xy);
	// 閾値以下だったら透明にする
	if (mask <= gSetting.threshold) {
		discard;
	}
	
	// edge部分の検出
	float edge = 1.0f - smoothstep(gSetting.threshold, gSetting.threshold + 0.03f, mask);
	output.color = gTexture.Sample(gSampler, input.texcoord);	
	// materialの色と合成
	output.color *= gSetting.color;
	// edge部分ならedgeColorを足す
	output.color.rgb += edge * gSetting.edgeColor.rgb;
	return output;
}