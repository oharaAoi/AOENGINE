struct VertexShaderOutput {
	float4 position : SV_POSITION;
	float3 texcoord : TEXCOORD0;
	float3 normal : NORMAL0;
	float4 worldPos : WORLDPOS0;
	float4 positionPrev : POSITIONPREV1;
};