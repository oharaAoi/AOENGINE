#include "Particle.hlsli"

struct PerView {
	float4x4 viewProjection;
	float4x4 billboardMat;
};

StructuredBuffer<CpuParticle> gParticles : register(t0);
ConstantBuffer<PerView> gPerView : register(b0);

struct VertexShaderInput {
	float4 position : POSITION0;
	float2 texcoord : TEXCOORD0;
};

VertexShaderOutput main(VertexShaderInput input, uint instanceId : SV_InstanceID) {
	VertexShaderOutput output;
	
	CpuParticle particle = gParticles[instanceId];
	output.position = mul(input.position, mul(particle.worldMat,gPerView.viewProjection));
	output.texcoord = input.texcoord;
	output.color = particle.color;
	
	return output;
}