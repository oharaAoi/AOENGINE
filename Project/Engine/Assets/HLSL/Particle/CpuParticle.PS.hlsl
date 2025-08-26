#include "Particle.hlsli"
#include "../Material.hlsli"

ConstantBuffer<Material> gMaterial : register(b0);
Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);
struct PixelShaderOutput {
	float4 color : SV_TARGET0;
	float4 motionVector : SV_TARGET1;
};

PixelShaderOutput main(VertexShaderOutput input) {
	PixelShaderOutput output;
	float4 transformedUV = mul(float4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
	float4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
	
	if (textureColor.a <= gMaterial.discardValue) {
		discard;
	}
	
	output.color.rgb = textureColor.rgb * gMaterial.color.rgb * input.color.rgb;
	output.color.a = textureColor.a * input.color.a * gMaterial.color.a;
	
	if (output.color.a <= gMaterial.discardValue) {
		discard;
	}
	
	output.motionVector = float4(0, 0, 0, 1);
	
	return output;
}