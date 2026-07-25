struct PixelShaderInput {
	float4 position : SV_POSITION;
	float2 texcoord : TEXCOORD0;
};

struct PixelShaderOutput {
	float4 color : SV_TARGET0;
	float depth : SV_DEPTH;
};

struct GridParameter {
	float4x4 inverseViewProjection;
	float4x4 viewProjection;
	float3 cameraPosition;
	float baseGridSize;
	float fadeStart;
	float fadeEnd;
	float2 padding;
};

ConstantBuffer<GridParameter> gGridParameter : register(b0);

float GetGridLine(float2 worldPosition, float spacing) {
	float2 gridPosition = worldPosition / spacing;
	float2 gridDerivative = max(fwidth(gridPosition), 0.00001f);
	float2 distanceToLine =
		abs(frac(gridPosition - 0.5f) - 0.5f) / gridDerivative;
	return 1.0f - saturate(min(distanceToLine.x, distanceToLine.y));
}

PixelShaderOutput main(PixelShaderInput input) {
	PixelShaderOutput output;

	float2 ndc = float2(
		input.texcoord.x * 2.0f - 1.0f,
		1.0f - input.texcoord.y * 2.0f);

	// DirectXのNDC深度範囲は0～1。
	float4 nearPositionH =
		mul(float4(ndc, 0.0f, 1.0f), gGridParameter.inverseViewProjection);
	float4 farPositionH =
		mul(float4(ndc, 1.0f, 1.0f), gGridParameter.inverseViewProjection);
	float3 nearPosition = nearPositionH.xyz / nearPositionH.w;
	float3 farPosition = farPositionH.xyz / farPositionH.w;
	float3 ray = farPosition - nearPosition;

	if (abs(ray.y) < 0.00001f) {
		discard;
	}

	// カメラレイとY=0平面との交点を求める。
	float intersection = -nearPosition.y / ray.y;
	if (intersection <= 0.0f) {
		discard;
	}

	float3 worldPosition = nearPosition + ray * intersection;
	float distanceFromCamera =
		length(worldPosition - gGridParameter.cameraPosition);

	// カメラの高さに応じて0.1, 1, 10, 100...のグリッドを滑らかに切り替える。
	float cameraHeight = max(abs(gGridParameter.cameraPosition.y), 0.001f);
	float logarithmicLevel =
		log10(cameraHeight / max(gGridParameter.baseGridSize, 0.0001f));
	float level = floor(logarithmicLevel);
	float levelBlend = smoothstep(0.2f, 0.8f, frac(logarithmicLevel));
	float fineSpacing = gGridParameter.baseGridSize * pow(10.0f, level);
	float coarseSpacing = fineSpacing * 10.0f;

	float fineLine = GetGridLine(worldPosition.xz, fineSpacing);
	float coarseLine = GetGridLine(worldPosition.xz, coarseSpacing);
	float majorLine = GetGridLine(worldPosition.xz, coarseSpacing * 10.0f);

	float minorAlpha = lerp(fineLine * 0.32f, coarseLine * 0.32f, levelBlend);
	float lineAlpha = max(minorAlpha, majorLine * 0.58f);
	float3 color = lerp(
		float3(0.72f, 0.72f, 0.72f),
		float3(0.98f, 0.98f, 0.98f),
		majorLine);

	// ワールドX軸は赤、Z軸は青。
	float axisDerivativeX = max(fwidth(worldPosition.z), 0.00001f);
	float axisDerivativeZ = max(fwidth(worldPosition.x), 0.00001f);
	float xAxis = 1.0f - saturate(abs(worldPosition.z) / axisDerivativeX);
	float zAxis = 1.0f - saturate(abs(worldPosition.x) / axisDerivativeZ);
	color = lerp(color, float3(0.80f, 0.16f, 0.16f), xAxis);
	color = lerp(color, float3(0.16f, 0.36f, 0.85f), zAxis);
	lineAlpha = max(lineAlpha, max(xAxis, zAxis) * 0.9f);

	float distanceFade = 1.0f - smoothstep(
		gGridParameter.fadeStart,
		gGridParameter.fadeEnd,
		distanceFromCamera);
	float horizonFade = smoothstep(
		0.015f, 0.12f, abs(normalize(ray).y));
	lineAlpha *= distanceFade * horizonFade;

	if (lineAlpha < 0.003f) {
		discard;
	}

	float4 clipPosition =
		mul(float4(worldPosition, 1.0f), gGridParameter.viewProjection);
	output.depth = saturate(clipPosition.z / clipPosition.w);
	output.color = float4(color, lineAlpha);
	return output;
}
