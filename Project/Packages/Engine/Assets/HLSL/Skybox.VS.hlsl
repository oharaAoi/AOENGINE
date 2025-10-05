#include "Skybox.hlsli"

struct WorldTransformMatrix {
	float4x4 world;
	float4x4 worldPrev;
	float4x4 worldInverseTranspose;
};

struct ViewProjectionMatrix {
	float4x4 view;
	float4x4 projection;
};

ConstantBuffer<WorldTransformMatrix> gWorldTransformMatrix : register(b0);
ConstantBuffer<ViewProjectionMatrix> gViewProjectionMatrix : register(b1);
ConstantBuffer<ViewProjectionMatrix> gViewProjectionMatrixPrev : register(b2);

struct VertexShaderInput {
	float4 position : POSITION0;
	float2 texcoord : TEXCOORD0;
	float3 normal : NORMAL0;
	float4 worldPos : WORLDPOS0;
	float3 tangent : TANGENT0;
};

VertexShaderOutput main(VertexShaderInput input) {
	VertexShaderOutput output;
	float4x4 WVP = mul(mul(gWorldTransformMatrix.world, gViewProjectionMatrix.view), gViewProjectionMatrix.projection);
	float4x4 prevWVP = mul(mul(gWorldTransformMatrix.worldPrev, gViewProjectionMatrixPrev.view), gViewProjectionMatrixPrev.projection);
	output.position = mul(input.position, WVP).xyww;
	output.texcoord = input.position.xyz;
	output.positionPrev = mul(input.position, prevWVP).xyww;
	return output;
}