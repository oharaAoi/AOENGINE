
SamplerState gSampler : register(s0);
Texture2D<float4> texA : register(t0);
Texture2D<float4> texB : register(t1);
RWTexture2D<float4> outputTex : register(u0);

[numthreads(16, 16, 1)]
void CSmain(uint3 id : SV_DispatchThreadID) {
	uint2 pix = id.xy;
    
	// --- output の解像度 ---
	uint outW, outH;
	outputTex.GetDimensions(outW, outH);

    // --- 0〜1 のUVを作る ---
	float2 uv = (pix + 0.5) / float2(outW, outH);

    // --- A と B は UV でサンプリングする ---
	float4 a = texA.SampleLevel(gSampler, uv, 0);
	float4 b = texB.SampleLevel(gSampler, uv, 0);
	
	float4 result = abs(a - b);

	outputTex[pix] = result;
}