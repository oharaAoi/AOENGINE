#include "Particle.hlsli"
#include "MatrixMath.hlsl"

struct PerView {
	float4x4 viewProjection;
	float4x4 billboardMat;
};

StructuredBuffer<Particle> gParticles : register(t0);
ConstantBuffer<PerView> gPerView : register(b0);

struct VertexShaderInput{
	float4 position : POSITION0;
	float2 texcoord : TEXCOORD0;
	float4 colot : COLOR0;
};

VertexShaderOutput main(VertexShaderInput input, uint instanceId : SV_InstanceID){
	VertexShaderOutput output;
	
	Particle particle = gParticles[instanceId];
	
	float4x4 worldMat = mul(
		mul(MakeScaleMat(gParticles[instanceId].scale), 
		MakeRotateMat(gParticles[instanceId].rotate)),
		MakeTranslateMat(gParticles[instanceId].translate)
	);

	output.position = mul(input.position, mul(worldMat, gPerView.viewProjection));
	output.texcoord = input.texcoord;
	output.color = particle.color;
	
	return output;
}