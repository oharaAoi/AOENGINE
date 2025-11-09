
struct UVParam {
	float2 uv;
};

Texture2D<float4> gInputTex : register(t0);
ConstantBuffer<UVParam> gUV : register(b0);
RWTexture2D<float4> gPreviewTex : register(u0);
SamplerState gSampler : register(s0);

[numthreads(16, 16, 1)]
void CSmain(uint3 id : SV_DispatchThreadID) {
	uint2 pix = id.xy;

	uint w, h;
	gPreviewTex.GetDimensions(w, h);

    // 基本UVを生成 (0~1)
	float2 uv = (pix + 0.5) / float2(w, h);

    // ★ Tiling → Offset の順番は Unity と同じ
	uv = uv + gUV.uv;

    // サンプリング（Wrap/Clamp/Mirror は SamplerState で決まる）
	float4 col = gInputTex.SampleLevel(gSampler, uv, 0);

	gPreviewTex[pix] = col;
}