#include "Particle.hlsli"
#include "MatrixMath.hlsl"

struct PerView {
	float4x4 viewProjection;
	float4x4 billboardMat;
};

struct VertexShaderInput {
	float4 position : POSITION0;
	float2 texcoord : TEXCOORD0;
};

StructuredBuffer<GpuParticle> gParticles : register(t0);
ConstantBuffer<PerView> gPerView : register(b0);

VertexShaderOutput main(VertexShaderInput input, uint instanceId : SV_InstanceID) {
	VertexShaderOutput output;
	GpuParticle particle = gParticles[instanceId];
	float4x4 worldMat = gPerView.billboardMat;
	worldMat[0] *= particle.scale.x;
	worldMat[1] *= particle.scale.y;
	worldMat[2] *= particle.scale.z;
	worldMat[3].xyz = particle.pos.xyz;
	
	output.position = mul(input.position, mul(worldMat, gPerView.viewProjection));
	output.texcoord = input.texcoord;
	output.color = particle.color;
	
	return output;
}