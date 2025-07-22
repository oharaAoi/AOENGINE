
struct Well {
	float4x4 skeletonSpaceMatrix;
	float4x4 skeletonSpaceInverseTransposeMatrix;
};

struct Vertex {
	float4 position;
	float2 texcoord;
	float3 normal;
	float4 worldPos;
	float3 tangent;
};

struct VertexInfluence {
	float4 weight;
	int4 index;
};

struct SkinningInformation {
	uint numVertices;
};


StructuredBuffer<Well> gMatrixPalette : register(t0);
StructuredBuffer<Vertex> gInputVertices : register(t1);
StructuredBuffer<VertexInfluence> gInfluences : register(t2);
// skinning計算後の頂点データ
RWStructuredBuffer<Vertex> gOutoutVertices : register(u0);
// skinningに関するちょっと情報
ConstantBuffer<SkinningInformation> gSkinningInformation : register(b0);

[numthreads(256, 1, 1)]
void CSmain(uint3 id : SV_DispatchThreadID) {
	uint vertexIndex = id.x;
	if (vertexIndex >= gSkinningInformation.numVertices)
		return;

	Vertex input = gInputVertices[vertexIndex];
	VertexInfluence influence = gInfluences[vertexIndex];

	float4x4 m0 = gMatrixPalette[influence.index.x].skeletonSpaceMatrix;
	float4x4 m1 = gMatrixPalette[influence.index.y].skeletonSpaceMatrix;
	float4x4 m2 = gMatrixPalette[influence.index.z].skeletonSpaceMatrix;
	float4x4 m3 = gMatrixPalette[influence.index.w].skeletonSpaceMatrix;

	float3x3 n0 = (float3x3) gMatrixPalette[influence.index.x].skeletonSpaceInverseTransposeMatrix;
	float3x3 n1 = (float3x3) gMatrixPalette[influence.index.y].skeletonSpaceInverseTransposeMatrix;
	float3x3 n2 = (float3x3) gMatrixPalette[influence.index.z].skeletonSpaceInverseTransposeMatrix;
	float3x3 n3 = (float3x3) gMatrixPalette[influence.index.w].skeletonSpaceInverseTransposeMatrix;

	Vertex skinned;
	skinned.texcoord = input.texcoord;
	skinned.tangent = input.tangent;
	skinned.worldPos = input.worldPos;
	
	skinned.position = mul(input.position, m0) * influence.weight.x +
	                   mul(input.position, m1) * influence.weight.y +
	                   mul(input.position, m2) * influence.weight.z +
	                   mul(input.position, m3) * influence.weight.w;
	skinned.position.w = 1.0f;

	skinned.normal = normalize(
		mul(input.normal, n0) * influence.weight.x +
		mul(input.normal, n1) * influence.weight.y +
		mul(input.normal, n2) * influence.weight.z +
		mul(input.normal, n3) * influence.weight.w
	);

	gOutoutVertices[vertexIndex] = skinned;
}