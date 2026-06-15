#include "Skybox.hlsli"
#include "Material.hlsli"

ConstantBuffer<Material> gMaterial : register(b0);
TextureCube<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct PixelShaderOutput {
	float4 color : SV_TARGET0;
	float4 motionVector : SV_TARGET1;
};


float2 ComputeMotionVector(float4 currentCS, float4 prevCS) {
    
	float2 currentNDC = currentCS.xy / currentCS.w;
	float2 prevNDC = prevCS.xy / prevCS.w;
	
	float2 currentSS = currentNDC * 0.5f + 0.5f;
	float2 prevSS = prevNDC * 0.5f + 0.5f;
	
	return currentSS - prevSS;
}

PixelShaderOutput main(VertexShaderOutput input) {
	PixelShaderOutput output;
	float4 textureColor = gTexture.Sample(gSampler, input.texcoord);
	output.color = textureColor * gMaterial.color;
	// モーションベクトル計算
	output.motionVector = float4(0, 0, 0, 1);
	return output;
}