#include "Object3dInstancing.hlsli"

// CPU側のTransformInstanceDataと同じ並びのinstance transform。
struct WorldTransformMatrix {
	float4x4 world;
	float4x4 worldPerv;
	float4x4 worldInverseTranspose;
};

struct ViewProjectionMatrix {
	float4x4 view;
	float4x4 projection;
};

StructuredBuffer<WorldTransformMatrix> gInstanceTransforms : register(t0);
ConstantBuffer<ViewProjectionMatrix> gViewProjectionMatrix : register(b1);
ConstantBuffer<ViewProjectionMatrix> gViewProjectionMatrixPrev : register(b2);

struct VertexShaderInput {
	float4 position : POSITION0;
	float2 texcoord : TEXCOORD0;
	float3 normal : NORMAL0;
	float4 tangent : TANGENT0;
};

// SceneRendererで同一MeshにまとめたinstanceをSV_InstanceIDで参照します。
VertexShaderOutput main(VertexShaderInput input, uint instanceId : SV_InstanceID) {
	VertexShaderOutput output;
	WorldTransformMatrix transform = gInstanceTransforms[instanceId];
	
	float4x4 WVP = mul(transform.world, mul(gViewProjectionMatrix.view, gViewProjectionMatrix.projection));
	float4x4 prevWVP = mul(transform.worldPerv, mul(gViewProjectionMatrixPrev.view, gViewProjectionMatrixPrev.projection));

	output.position = mul(input.position, WVP);
	output.positionNDC = mul(input.position, WVP);
	output.positionPrev = mul(input.position, prevWVP);
	output.texcoord = input.texcoord;
	output.normal = normalize(mul(input.normal, (float3x3) transform.worldInverseTranspose));
	output.worldPos = mul(input.position, transform.world);
	output.instanceId = instanceId;
	return output;
}
