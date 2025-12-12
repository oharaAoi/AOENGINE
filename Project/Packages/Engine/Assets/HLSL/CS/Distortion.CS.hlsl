
struct DistortionParam {
	float2 tiling;
	float2 scroll;
	float strength;
	float time;
};

SamplerState gSampler : register(s0);
ConstantBuffer<DistortionParam> gDistortion : register(b0);
Texture2D<float4> gBaseTex : register(t0);
Texture2D<float4> gNoiseTex : register(t1);
RWTexture2D<float4> outputTex : register(u0);

[numthreads(16, 16, 1)]
void CSmain(uint3 id : SV_DispatchThreadID) {
	uint2 pix = id.xy;

	uint outW, outH;
	outputTex.GetDimensions(outW, outH);

	if (pix.x >= outW || pix.y >= outH)
		return;

    // 出力サイズ基準の UV（0〜1）
	float2 uv = (pix + 0.5f) / float2(outW, outH);

    // Noise 用 UV（tiling & scroll）
	float2 noiseUV = uv * gDistortion.tiling
                   + gDistortion.scroll * gDistortion.time;

    // ノイズから [-1,1] のオフセットベクトルを作る
	float2 dist = gNoiseTex.SampleLevel(gSampler, noiseUV, 0).rg;
	
	// ★ バイアス除去（最重要）
	dist = (dist - 0.5f) * 2.0f; // 平均を0にする
	float2 offset = dist * gDistortion.strength;

    // 歪んだ UV
	float2 warpedUV = uv + offset;

    // はみ出しを Clamp
	warpedUV = saturate(warpedUV);

    // Base を歪んだ UV でサンプル（解像度差は Sample 側が吸収してくれる）
	float4 color = gBaseTex.SampleLevel(gSampler, warpedUV, 0);

	outputTex[pix] = color;
}