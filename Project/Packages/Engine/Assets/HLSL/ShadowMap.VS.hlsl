#include "Object3d.hlsli"

struct WorldTransformMatrix {
	float4x4 world;
	float4x4 worldPerv;
	float4x4 worldInverseTranspose;
};

struct ViewProjectionMatrix {
	float4x4 view;
	float4x4 projection;
};

ConstantBuffer<WorldTransformMatrix> gWorldTransformMatrix : register(b0);
ConstantBuffer<ViewProjectionMatrix> gLightViewProjectionMatrix : register(b1);

struct VertexShaderInput {
	float4 position : POSITION0;
	float2 texcoord : TEXCOORD0;
	float3 normal : NORMAL0;
	float4 worldPos : WORLDPOS0;
	float3 tangent : TANGENT0;
};

VertexShaderOutput main(VertexShaderInput input) {
	VertexShaderOutput output;

    // ---- current ----
	float4x4 viewProj = mul(gLightViewProjectionMatrix.view, gLightViewProjectionMatrix.projection);
	float4 worldPos = mul(input.position, gWorldTransformMatrix.world);
	float4 clipPos = mul(worldPos, viewProj);

	output.position = clipPos;
	output.positionNDC = clipPos;
	
	output.texcoord = input.texcoord;
	output.worldPos = worldPos;
	output.normal =
        normalize(mul(input.normal,
            (float3x3) gWorldTransformMatrix.worldInverseTranspose));

	return output;
}