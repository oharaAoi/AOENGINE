#include "Object3d.hlsli"

struct Material {
	float4 color;
	int enableLighting;
	float4x4 uvTransform;
	float shininess; // 光沢度
};

ConstantBuffer<Material> gMaterial : register(b0);
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
	float4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
	float4 noiseColor = gNoiseTexture.Sample(gSampler, transformedUV.xy);
	
	if (textureColor.a == 0.0) {
		discard;
	}
	
	if (gMaterial.enableLighting == 0) {
		output.color = gMaterial.color * textureColor;
		if (output.color.a == 0.0) {
			discard;
		}
		return output;
	}
	float3 normal = normalize(input.normal);
	
	output.color.rgb = gMaterial.color.rgb * textureColor.rbg * noiseColor.rbg;
	
	output.color.a = gMaterial.color.a * textureColor.a;
	output.color = clamp(output.color, 0.0f, 1.0f);
	
	if (output.color.a == 0.0) {
		discard;
	}
	
	return output;
}