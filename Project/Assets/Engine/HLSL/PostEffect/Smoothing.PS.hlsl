#include "ProcessedScene.hlsli"

struct Setting {
	int size;
};

ConstantBuffer<Setting> gSmoothSetting : register(b0);
Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct PixelShaderOutput {
	float4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input) {
	PixelShaderOutput output;
    
	uint width, height;
	gTexture.GetDimensions(width, height);
	float2 uvStepSize = float2(rcp((float) width), rcp((float) height));
    
	int n = max(gSmoothSetting.size, 1);
	if ((n & 1) == 0) {
		n -= 1;
	} // 奇数に揃える
	int r = n / 2;

	float3 sum = 0.0f.xxx;

    [loop]
	for (int y = -r; y <= r; ++y) {
        [loop]
		for (int x = -r; x <= r; ++x) {
			float2 texcoord = input.texcoord + float2(x, y) * uvStepSize;
			float3 fetchColor = gTexture.Sample(gSampler, texcoord).rgb;
			sum += fetchColor;
		}
	}
    
	float invCount = 1.0f / (n * n);
	output.color = float4(sum * invCount, 1.0f);
	return output;
}