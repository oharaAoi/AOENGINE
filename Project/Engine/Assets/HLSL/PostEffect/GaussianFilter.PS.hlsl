#include "ProcessedScene.hlsli"

struct Setting {
	float deviation;
	int size;
};

ConstantBuffer<Setting> gGaussianSetting : register(b0);
Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct PixelShaderOutput {
	float4 color : SV_TARGET0;
};

static const float PI = 3.141592f;

float Gauss(float x, float y, float sigma) {
	float exponent = -(x * x + y * y) * rcp(2.0f * sigma * sigma);
	float denominator = 2.0f * PI * sigma * sigma;
	return exp(exponent) * rcp(denominator);

}

PixelShaderOutput main(VertexShaderOutput input) {
	PixelShaderOutput output;
	
	int n = max(gGaussianSetting.size, 1);
	if ((n & 1) == 0) {
		n -= 1;
	} // 奇数に揃える
	int r = n / 2;
	
	 // テクスチャサイズとUVステップ
	uint width, height;
	gTexture.GetDimensions(width, height);
	float2 uvStepSize = float2(1.0f / width, 1.0f / height);

	float3 sum = 0.0f.xxx;
	float weightSum = 0.0f;
	float sigma = max(gGaussianSetting.deviation, 0.0001f);

    // n x n ガウシアンフィルタ計算
    [loop]
	for (int y = -r; y <= r; ++y) {
        [loop]
		for (int x = -r; x <= r; ++x) {
			float w = Gauss(x, y, sigma); // 重み
			float2 uv = input.texcoord + float2(x, y) * uvStepSize;
			float3 c = gTexture.Sample(gSampler, uv).rgb;
			sum += c * w;
			weightSum += w;
		}
	}

    // 正規化
	output.color = float4(sum / weightSum, 1.0f);
	return output;
}
