
struct Material {
	float4 color;
	int enableLighting;
	float4x4 uvTransform;
	float shininess; // 光沢度
	float discardValue;
};