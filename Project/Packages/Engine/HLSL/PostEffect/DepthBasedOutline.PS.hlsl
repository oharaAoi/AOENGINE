#include "ProcessedScene.hlsli"

struct Setting {
	float4x4 projectionInverse;
	float edgeGrain;
};

ConstantBuffer<Setting> gSetting : register(b0);
Texture2D<float4> gTexture : register(t0);
Texture2D<float> gDepthTexture : register(t1);
SamplerState gSampler : register(s0);
SamplerState gSamplerPoint : register(s1);

struct PixelShaderOutput {
	float4 color : SV_TARGET0;
};

static const float2 kIndex3x3[3][3] = {
	{ { -1.0f, -1.0f }, { 0.0f, -1.0f }, { 1.0f, -1.0f } },
	{ { -1.0f, 0.0f }, { 0.0f, 0.0f }, { 1.0f, 0.0f } },
	{ { -1.0f, 1.0f }, { 0.0f, 1.0f }, { 1.0f, 1.0f } }
};

static const float kPrewittHorizontalKernel[3][3] = {
	{ -1.0f / 6.0f, 0.0f, 1.0f / 6.0f },
	{ -1.0f / 6.0f, 0.0f, 1.0f / 6.0f },
	{ -1.0f / 6.0f, 0.0f, 1.0f / 6.0f }
};

static const float kPrewittVerticalKernel[3][3] = {
	{ -1.0f / 6.0f, -1.0f / 6.0f, -1.0f / 6.0f },
	{ 0, 0, 0 },
	{ 1.0f / 6.0f, 1.0f / 6.0f, 1.0f / 6.0f },
};

float SampleViewZ(float2 uv) {
	float depth = gDepthTexture.Sample(gSamplerPoint, uv); // [0,1] device depth
	float2 clipXY = uv * 2.0f - 1.0f;
	float clipZ = depth * 2.0f - 1.0f;
	float4 clipPos = float4(clipXY, clipZ, 1.0f);
	float4 viewPos = mul(clipPos, gSetting.projectionInverse);
	return viewPos.z / viewPos.w; // eye-space z
}

PixelShaderOutput main(VertexShaderOutput input) {
	PixelShaderOutput o;
	uint w, h;
	gTexture.GetDimensions(w, h);
	float2 step = float2(1.0 / w, 1.0 / h);

	float centerZ = SampleViewZ(input.texcoord);
	float2 grad = 0;

    [unroll]
	for (int iy = 0; iy < 3; ++iy) {
        [unroll]
		for (int ix = 0; ix < 3; ++ix) {
			float2 uv = input.texcoord + kIndex3x3[iy][ix] * step;
			float z = SampleViewZ(uv);
			float dz = z - centerZ; // relative
			grad.x += dz * kPrewittHorizontalKernel[iy][ix];
			grad.y += dz * kPrewittVerticalKernel[iy][ix];
		}
	}

    // 距離スケール（任意調整）
	float scale = rcp(max(abs(centerZ), 1e-3)); // 遠いほど緩く
	float edge = saturate(length(grad) * scale * gSetting.edgeGrain); // edgeGainで調整

	float3 src = gTexture.Sample(gSampler, input.texcoord).rgb;
	float3 lineColor = float3(0.6, 0.1f, 0.1f); // 黒線
	o.color = float4(lerp(src, lineColor, edge), 1);
	return o;
}