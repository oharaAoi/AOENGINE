#include "Object3d.hlsli"

struct WorldTransformMatrix {
	float4x4 world;
};

struct ViewProjectionMatrix {
	float4x4 view;
	float4x4 projection;
};

StructuredBuffer<WorldTransformMatrix> gInstanceTransforms : register(t0);
ConstantBuffer<ViewProjectionMatrix> gLightViewProjectionMatrix : register(b1);

struct VertexShaderInput {
	float4 position : POSITION0;
	float2 texcoord : TEXCOORD0;
	float3 normal : NORMAL0;
	float4 tangent : TANGENT0;
};

VertexShaderOutput main(VertexShaderInput input, uint instanceId : SV_InstanceID) {
	VertexShaderOutput output;
	WorldTransformMatrix transform = gInstanceTransforms[instanceId];

	float4x4 viewProj = mul(gLightViewProjectionMatrix.view, gLightViewProjectionMatrix.projection);
	float4 worldPos = mul(input.position, transform.world);
	float4 clipPos = mul(worldPos, viewProj);

	output.position = clipPos;
	output.positionNDC = clipPos;
	output.texcoord = input.texcoord;
	output.worldPos = worldPos;
	output.normal = input.normal;
	return output;
}
