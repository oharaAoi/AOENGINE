
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

	uint w, h;
	outputTex.GetDimensions(w, h);

    // 画像サイズ外は処理しないようにガード
	if (pix.x >= w || pix.y >= h)
		return;

    // 0〜1 のUV
	float2 uv = (pix + 0.5f) / float2(w, h);

    // 歪みマップ用のUV（タイリング＋スクロール）
	float2 distUV = uv * gDistortion.tiling + gDistortion.scroll * gDistortion.time;

    // 歪みマップのサンプル
    // R,G を [-1,1] に変換してオフセットとして使う
	float2 distSample = gNoiseTex.SampleLevel(gSampler, distUV, 0).rg;
	float2 offset = (distSample * 2.0f - 1.0f) * gDistortion.strength;

    // 元画像のUVにオフセットを加算
	float2 warpedUV = uv + offset;

    // 画面外にはみ出さないように Clamp
	warpedUV = saturate(warpedUV);

    // 歪んだUVで元画像をサンプリング
	float4 color = gBaseTex.SampleLevel(gSampler, warpedUV, 0);

    // 出力
	outputTex[pix] = color;
}