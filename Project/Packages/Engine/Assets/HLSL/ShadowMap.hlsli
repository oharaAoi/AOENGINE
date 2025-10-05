
float DrawShadow(Texture2D<float> _texture, SamplerState _sampler, float4 _lightPos) {
	float2 shadowUV = _lightPos.xy * 0.5f + 0.5f;
	shadowUV.y = 1.0f - shadowUV.y;
	if (shadowUV.x < 0 || shadowUV.x > 1 || shadowUV.y < 0 || shadowUV.y > 1) {
		return 0.0f; // 影外扱い
	}

	float shadowDepth = _texture.Sample(_sampler, shadowUV).r;
	float currentDepth = _lightPos.z * 0.5f + 0.5f;

	float bias = 0.001f;
	return (currentDepth - bias > shadowDepth) ? 1.0f : 0.0f; // 影中なら1.0
}