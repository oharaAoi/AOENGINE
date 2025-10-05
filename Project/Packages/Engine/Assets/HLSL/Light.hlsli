
struct DirectionalLight {
	float4x4 lightViewProj;
	float4 color;
	float3 direction;
	float3 eyePos; // 視点の位置
	float intensity;
	float limPower;
};

struct PointLight {
	float4x4 lightViewProj;
	float4 color; // ライトの色
	float3 position; // ライトの位置
	float3 eyePos;
	float intensity; // 輝度
	float radius; // 最大距離
	float decay; // 減衰率
};

struct SpotLight {
	float4x4 lightViewProj;
	float4 color; // ライトの色
	float3 position; // ライトの位置
	float3 eyePos; // 視点の位置
	float intensity; // 輝度
	float3 direction; // 方向
	float distance; // ライトの届く最大距離
	float decay; // 減衰率
	float cosAngle; // スポットライトの余弦
	float cosFalloffStart;
};


//==========================================
// Lambert
//==========================================
float3 Lambert(float NdotL, float3 textureColor) {
	float3 resultColor = textureColor * NdotL;
	return resultColor;
}

//==========================================
// HalfLambert
//==========================================
float3 HalfLambert(float NdotL, float3 lightColor) {
	float cos = (pow(NdotL * 0.5f + 0.5f, 2.0f)) / 3.1415f;
	float3 diffuse = lightColor * cos;
	
	return diffuse;
}

//==========================================
//　phong
//==========================================
float3 Phong(float RDotE, float3 lightColor, float shininess) {
	// 反射強度
	float specularPow = pow(saturate(RDotE), shininess);
	// 鏡面反射
	float3 specular = lightColor.rbg * specularPow;

	return specular;
}

//==========================================
//　BlinnPhong
//==========================================
float3 BlinnPhong(float NDotH, float3 lightColor, float shininess) {
	// 反射強度
	float specularPow = pow(saturate(NDotH), shininess);
	// 鏡面反射
	float3 specular = lightColor.rbg * specularPow * float3(1.0f, 1.0f, 1.0f);

	return specular;
}

//================================================================================================//
//
//	lightの計算
//
//==============================================================================================gSpotLight==//

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　directionalLight
//////////////////////////////////////////////////////////////////////////////////////////////////

float3 DirectionalLighting(float NdotL, float NdotH, float shininess, float3 lightColor, float3 materialColor, float3 textureColor) {
	float3 diffuse = HalfLambert(NdotL, lightColor) * materialColor * textureColor;
	float3 speculer = BlinnPhong(NdotH, lightColor, shininess);
	
	return diffuse + speculer;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　pointLighting
//////////////////////////////////////////////////////////////////////////////////////////////////

float3 PointLighting(float NdotL, float pNdotH, float shininess, float3 lightColor, float3 materialColor, float3 textureColor) {
	float3 diffuse = HalfLambert(NdotL, lightColor) * materialColor * textureColor;
	float3 speculer = BlinnPhong(pNdotH, lightColor, shininess);
	
	return diffuse + speculer;
}

float3 SpotLighting(ConstantBuffer<SpotLight> spotLight, float NdotL, float NdotH, float shininess, float3 inputWorldPos, float3 materialColor, float3 textureColor) {
	float3 spotLightDirectionOnSurface = normalize(inputWorldPos - spotLight.position);
	// Falloff
	float cosAngle = dot(spotLightDirectionOnSurface, spotLight.direction);
	float falloffFactor = saturate((cosAngle - spotLight.cosAngle) / (spotLight.cosFalloffStart - spotLight.cosAngle));
	// 距離による減衰
	float distanceSpot = length(spotLight.position - inputWorldPos);
	float attenuationFactor = pow(saturate(-distanceSpot / spotLight.distance + 1.0f), spotLight.decay);
	// スポットライトのカラー
	float3 spotColor = spotLight.color.rgb * spotLight.intensity * attenuationFactor * falloffFactor;
	
	// lambert
	float3 spotDiffuse;
	spotDiffuse = HalfLambert(NdotL, spotColor.rgb) * materialColor.rgb * textureColor.rgb;
	
	// phong
	float3 spotSpeculer = BlinnPhong(NdotH, spotColor.rgb, shininess) * textureColor.rgb;
	
	return (spotDiffuse + spotSpeculer) * spotLight.intensity;
}