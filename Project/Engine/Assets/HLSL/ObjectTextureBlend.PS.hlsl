#include "Object3d.hlsli"
#include "Material.hlsli"

struct NoiseUV {
	float4x4 uvTransform;
};

ConstantBuffer<Material> gMaterial : register(b0);
ConstantBuffer<NoiseUV> gNoiseUV : register(b1);
Texture2D<float4> gTexture : register(t0);
Texture2D<float4> gNoiseTexture : register(t1);
SamplerState gSampler : register(s0);
struct PixelShaderOutput {
	float4 color : SV_TARGET0;
};


//================================================================================================//
//
//	main
//
//================================================================================================//

PixelShaderOutput main(VertexShaderOutput input) {
	PixelShaderOutput output;
	float4 transformedUV = mul(float4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
	float4 noiseUV = mul(float4(input.texcoord, 0.0f, 1.0f), gNoiseUV.uvTransform);
	float4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
	float4 noiseColor = gNoiseTexture.Sample(gSampler, noiseUV.xy);
	
	if (textureColor.a <= gMaterial.discardValue) {
		discard;
	}
	
	if (gMaterial.enableLighting == 0) {
		output.color = gMaterial.color * textureColor;
		if (output.color.a <= gMaterial.discardValue) {
			discard;
		}
		return output;
	}
	float3 normal = normalize(input.normal);
	
	output.color.rgb = gMaterial.color.rgb * textureColor.rbg * noiseColor.rgb;
	
	output.color.a = gMaterial.color.a * textureColor.a;
	output.color = clamp(output.color, 0.0f, 1.0f);
	
	if (output.color.a <= gMaterial.discardValue) {
		discard;
	}
	
	return output;
}