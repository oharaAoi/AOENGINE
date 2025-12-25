#include "PBR.hlsli"

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
ConstantBuffer<ViewProjectionMatrix> gViewProjectionMatrix : register(b1);
ConstantBuffer<ViewProjectionMatrix> gViewProjectionMatrixPrev : register(b2);

struct VertexShaderInput{
	float4 position : POSITION0;
	float2 texcoord : TEXCOORD0;
	float3 normal : NORMAL0;
	float4 tangent : TANGENT;
};

VertexShaderOutput main(VertexShaderInput input)
{
	VertexShaderOutput output;
	// WVPの生成
	float4x4 WVP = mul(gWorldTransformMatrix.world, mul(gViewProjectionMatrix.view, gViewProjectionMatrix.projection));
	float4x4 prevWVP = mul(gWorldTransformMatrix.worldPerv, mul(gViewProjectionMatrixPrev.view, gViewProjectionMatrixPrev.projection));
	
	output.position = mul(input.position, WVP);
	output.positionNDC = mul(input.position, WVP);
	output.positionPrev = mul(input.position, prevWVP);
	output.texcoord = input.texcoord;
	output.normal = normalize(mul(input.normal, (float3x3) gWorldTransformMatrix.worldInverseTranspose));
	output.worldPos = mul(input.position, gWorldTransformMatrix.world);
	float3 N = normalize(mul(input.normal, (float3x3) gWorldTransformMatrix.worldInverseTranspose));
	float3 T = normalize(mul(input.tangent.xyz, (float3x3) gWorldTransformMatrix.worldInverseTranspose));

	// Orthonormalize
	T = normalize(T - N * dot(N, T));

	// Bitangent
	float3 B = cross(N, T) * input.tangent.w;

	// TBN
	float3x3 TBN = float3x3(T, B, N);
	output.tangentMat = TBN;
	
	return output;
}