struct VertexShaderInput {
	float4 start : POSITION0;
	float4 end : TANGENT0;
	float4 color : COLOR0;
	float3 lineData : NORMAL0;
	float2 viewportSize : TEXCOORD0;
};

struct VertexShaderOutput {
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

VertexShaderOutput main(VertexShaderInput input) {
	VertexShaderOutput output;
	float4 position = lerp(input.start, input.end, input.lineData.x);
	float2 startNdc = input.start.xy / input.start.w;
	float2 endNdc = input.end.xy / input.end.w;
	float2 screenDirection = float2(
		(endNdc.x - startNdc.x) * input.viewportSize.x,
		-(endNdc.y - startNdc.y) * input.viewportSize.y);
	float directionLength = length(screenDirection);
	float2 normal = directionLength > 0.0001f
		? float2(-screenDirection.y, screenDirection.x) / directionLength
		: float2(0.0f, 1.0f);
	float2 ndcPerPixel = float2(2.0f / input.viewportSize.x, -2.0f / input.viewportSize.y);
	position.xy += normal * input.lineData.y * input.lineData.z * ndcPerPixel * position.w;
	output.position = position;
	output.color = input.color;
	return output;
}
