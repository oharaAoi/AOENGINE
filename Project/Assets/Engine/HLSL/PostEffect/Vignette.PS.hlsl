#include "ProcessedScene.hlsli"

struct Vignette {
	float4 color;
	float scale;
	float power;
};

ConstantBuffer<Vignette> gSetting : register(b0);
Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct PixelShaderOutput {
	float4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input) {
	PixelShaderOutput output;

    // 元のテクスチャカラー取得
	float4 texColor = gTexture.Sample(gSampler, input.texcoord);

    // ビネット係数計算（中心ほど強く、端ほど弱く）
	float2 correct = input.texcoord * (1.0f - input.texcoord.yx);
	float vignetteFactor = correct.x * correct.y * gSetting.scale;
	vignetteFactor = saturate(pow(vignetteFactor, gSetting.power));

    // ビネット色を適用（色を gSetting.color に寄せていく）
	output.color.rgb = lerp(gSetting.color.rgb, texColor.rgb, vignetteFactor);
	output.color.a = texColor.a;

	return output;
}