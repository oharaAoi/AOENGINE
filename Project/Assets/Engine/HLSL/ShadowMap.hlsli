
float DrawShadow(Texture2D<float> _texture, SamplerComparisonState _sampler, float4 _lightClipPos, float3 _lightDir, float3 _normal) {
	 // clip → ndc
	float3 ndc = _lightClipPos.xyz / _lightClipPos.w;
	
    // ndc → uv
	float2 uv = ndc.xy * 0.5f + 0.5f;
	uv.y = 1.0f - uv.y;

    // 範囲外は「影なし」
	if (any(uv < 0.0f) || any(uv > 1.0f))
		return 1.0f;

    // ndc.z → [0,1] 深度
	float depth = _lightClipPos.z / _lightClipPos.w;

    // slope-scaled bias（符号統一が重要）
	float ndotl = saturate(dot(normalize(_normal), normalize(_lightDir))); // lightDir は「表面→光」方向で統一
	float bias = max(0.0025f,  0.02f * (1.0f - ndotl));

    // Cmp: shadowMapDepth < depth ? 0 : 1 をハードウェアでやる
	return _texture.SampleCmpLevelZero(_sampler, uv, depth + bias);
}