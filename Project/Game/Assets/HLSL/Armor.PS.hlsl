struct VertexShaderOutput {
	float4 position : SV_POSITION;
	float2 texcoord : TEXCOORD0;
	float3 normal : NORMAL0;
	float4 worldPos : WORLDPOS0;
	float4 positionNDC : POSITIONNDC0;
	float4 positionPrev : POSITIONPREV0;
};

struct Dissolve {
	float4x4 uvTransform[3];
	float4 color;
	float4 edgeColor;
	float threshold;
};

ConstantBuffer<Dissolve> gSetting : register(b0);
Texture2D<float4> gTexture : register(t0);
Texture2D<float> gMaskTexture1 : register(t1);
Texture2D<float> gMaskTexture2 : register(t2);
Texture2D<float> gMaskTexture3 : register(t3);
SamplerState gSampler : register(s0);

struct PixelShaderOutput {
	float4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input) {
	PixelShaderOutput output;
	
	float4 transformedUV1 = mul(float4(input.texcoord, 0.0f, 1.0f), gSetting.uvTransform[0]);
	float4 transformedUV2 = mul(float4(input.texcoord, 0.0f, 1.0f), gSetting.uvTransform[1]);
	float4 transformedUV3 = mul(float4(input.texcoord, 0.0f, 1.0f), gSetting.uvTransform[2]);
	// uvを簡易的にずらす
	float2 uv1 = transformedUV1.xy * 1.0;
	float2 uv2 = transformedUV2.xy * 2.0;
	float2 uv3 = transformedUV3.xy * 4.0;
	// maskを計算する
	float mask1 = gMaskTexture1.Sample(gSampler, uv1).r;
	float mask2 = gMaskTexture2.Sample(gSampler, uv2).r;
	float mask3 = gMaskTexture3.Sample(gSampler, uv3).r;
	
	float combinedMask = (mask1 + mask2 * 0.5 + mask3 * 0.25) / 1.75;
	float alphaMask = smoothstep(gSetting.threshold, gSetting.threshold + 0.05f, combinedMask);
	if (alphaMask < 0.01f) {
		discard;
	}
	
	if (mask1 <= gSetting.threshold) {
		discard;
	}
	if (mask2 <= gSetting.threshold) {
		discard;
	}
	if (mask3 <= gSetting.threshold) {
		discard;
	}
	
	 // sin波でマーブル模様に変換
	float distortion = sin(transformedUV1.y * 6.0 + combinedMask * 8.0);
	float marble = sin(distortion);
	marble = marble * 0.5 + 0.5;
	
	if (marble <= gSetting.threshold) {
		discard;
	}

	// edge部分の検出
		float edge = 1.0f - smoothstep(gSetting.threshold, gSetting.threshold + 0.05f, marble);
	edge *= 8.0f;
	output.color = gTexture.Sample(gSampler, input.texcoord);
	// materialの色と合成
	output.color *= gSetting.color * 3.0f;
	// edge部分ならedgeColorを足す
	output.color.rgb += edge * gSetting.edgeColor.rgb;
	return output;
}