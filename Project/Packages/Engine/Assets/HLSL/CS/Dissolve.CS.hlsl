
struct DissolveParam {
	float4 color;
	float threshold;
	float edgeWidth;
};

SamplerState gSampler : register(s0);
ConstantBuffer<DissolveParam> gDissolve : register(b0);
Texture2D<float4> gBaseTex : register(t0);
Texture2D<float4> gNoiseTex : register(t1);
RWTexture2D<float4> gOutputTex : register(u0);

[numthreads(16, 16, 1)]
void CSmain(uint3 id : SV_DispatchThreadID) {
	uint2 pix = id.xy;

	// ------------------------
	// outputのuvを取得
	// ------------------------
	uint outW, outH;
	gOutputTex.GetDimensions(outW, outH);

	if (pix.x >= outW || pix.y >= outH)
		return;
	
	float2 uv = (pix + 0.5f) / float2(outW, outH);

	// ------------------------
	// baseのuvを取得
	// ------------------------
	uint baseW, baseH;
	gBaseTex.GetDimensions(baseW, baseH);
	float2 uvBase = uv * float2(outW, outH) / float2(baseW, baseH);
	
	uint2 basePix = pix * uint2(baseW, baseH) / uint2(outW, outH);
	
	// ------------------------
	// noiseのuvを取得
	// ------------------------
	uint noiseW, noiseH;
	gNoiseTex.GetDimensions(noiseW, noiseH);
	float2 uvNoise = uv * float2(outW, outH) / float2(noiseW, noiseH);
	
	// ------------------------
	// サンプリング
	// ------------------------
    // 元画像
	//float4 src = gBaseTex.SampleLevel(gSampler, uvBase, 0);
	float4 src = gBaseTex.Load(int3(basePix, 0));
    // ノイズ（0〜1）
	float noise = gNoiseTex.SampleLevel(gSampler, uvNoise, 0).r;

	// ------------------------
	// ディゾルブ
	// ------------------------
    // noise が threshold より小さければ消える
	float dissolveValue = noise - gDissolve.threshold;

	if (dissolveValue < 0.0f) {
        // まず完全に消す
		gOutputTex[pix] = float4(0, 0, 0, 0);
		return;
	}

    // ここから "消える境界線（エッジ）" を作る
	float edge = smoothstep(0.0, gDissolve.edgeWidth, dissolveValue);

    // エッジカラーの強さ（中央を 1 に）
	float edgeMask = 1.0 - edge;

    // 元色とエッジカラーをブレンド
	float3 color = lerp(gDissolve.color.rgb, src.rgb, edge);

    // 出力
	float alpha = src.a * edge; // アルファも滑らかに減衰
	gOutputTex[pix] = float4(color, alpha);
}