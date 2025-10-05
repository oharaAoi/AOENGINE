#include "ProcessedScene.hlsli"

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct PixelShaderOutput {
	float4 color : SV_TARGET0;
};

float3 ReinhardToneMap(float3 color) {
	return color / (color + 1.0);
}

float3 RRTAndODTFit(float3 v) {
	float3 a = v * (v + 0.0245786f) - 0.000090537f;
	float3 b = v * (0.983729f * v + 0.4329510f) + 0.238081f;
	return a / b;
}

float3 ACESFilmToneMap(float3 color) {
    // sRGB => ACES Input
	const float3x3 ACESInputMat = float3x3(
        0.59719, 0.35458, 0.04823,
        0.07600, 0.90834, 0.01566,
        0.02840, 0.13383, 0.83777
    );

    // ACES Output => sRGB
	const float3x3 ACESOutputMat = float3x3(
         1.60475, -0.53108, -0.07367,
        -0.10208, 1.10813, -0.00605,
        -0.00327, -0.07276, 1.07602
    );

    // Input color to ACES
	color = mul(ACESInputMat, color);

    // Apply curve
	color = RRTAndODTFit(color);

    // Back to sRGB
	color = mul(ACESOutputMat, color);

    // Clamp
	return saturate(color);
}

PixelShaderOutput main(VertexShaderOutput input) {
	PixelShaderOutput output;
	float3 hdrColor = gTexture.Sample(gSampler, input.texcoord).rgb;
	float3 toneMappedColor = ACESFilmToneMap(hdrColor);
	output.color = float4(toneMappedColor, 1.0);
	return output;
}