#include "Object3d.hlsli"
#include "Light.hlsli"

struct Material {
	float4 color;
	int enableLighting;
	float4x4 uvTransform;
	float shininess; // 光沢度
};

ConstantBuffer<Material> gMaterial : register(b0);
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);
ConstantBuffer<PointLight> gPointLight : register(b2);
ConstantBuffer<SpotLight> gSpotLight : register(b3);
Texture2D<float4> gTexture : register(t0);
Texture2D<float> gShadowMap : register(t1);
SamplerState gSampler : register(s0);
SamplerState gSamplerPoint : register(s1);

struct PixelShaderOutput {
	float4 color : SV_TARGET0;
	float4 motionVector : SV_TARGET1;
};

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　spotLighting
//////////////////////////////////////////////////////////////////////////////////////////////////

float3 SpotLighting(ConstantBuffer<SpotLight> spotLight, float NdotL, float NdotH, float3 inputWorldPos, float3 materialColor, float3 textureColor) {
	float3 spotLightDirectionOnSurface = normalize(inputWorldPos - gSpotLight.position);
	// Falloff
	float cosAngle = dot(spotLightDirectionOnSurface, gSpotLight.direction);
	float falloffFactor = saturate((cosAngle - gSpotLight.cosAngle) / (gSpotLight.cosFalloffStart - gSpotLight.cosAngle));
	// 距離による減衰
	float distanceSpot = length(gSpotLight.position - inputWorldPos);
	float attenuationFactor = pow(saturate(-distanceSpot / gSpotLight.distance + 1.0f), gSpotLight.decay);
	// スポットライトのカラー
	float3 spotColor = gSpotLight.color.rgb * gSpotLight.intensity * attenuationFactor * falloffFactor;
	
	// lambert
	float3 spotDiffuse;
	spotDiffuse = HalfLambert(NdotL, spotColor.rgb) * gMaterial.color.rgb * textureColor.rgb;
	
	// phong
	float3 spotSpeculer = BlinnPhong(NdotH, spotColor.rgb, gMaterial.shininess) * textureColor.rgb;
	
	return (spotDiffuse + spotSpeculer) * spotLight.intensity;
}

float2 ComputeMotionVector(float4 currentCS, float4 prevCS) {
    
	float2 currentNDC = currentCS.xy / currentCS.w;
	float2 prevNDC = prevCS.xy / prevCS.w;
	
	float2 currentSS = currentNDC;
	float2 prevSS = prevNDC ;
	
	float2 diff = currentSS - prevSS;
	
	return diff;
}

float ShadowBias(float4 lightPos) {
	float2 shadowUV = lightPos.xy * 0.5f + 0.5f;
	shadowUV.y = 1.0f - shadowUV.y;
	if (shadowUV.x < 0 || shadowUV.x > 1 || shadowUV.y < 0 || shadowUV.y > 1) {
		return 0.0f; // 影外扱い
	}

	float shadowDepth = gShadowMap.Sample(gSamplerPoint, shadowUV).r;
	float currentDepth = lightPos.z * 0.5f + 0.5f;

	float bias = 0.001f;
	return (currentDepth - bias > shadowDepth) ? 1.0f : 0.0f; // 影中なら1.0
}

//================================================================================================//
//	main
//================================================================================================//

PixelShaderOutput main(VertexShaderOutput input) {
	PixelShaderOutput output = (PixelShaderOutput) 0;
	float4 transformedUV = mul(float4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
	float4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
	
	// モーションベクトル計算
	float2 mv = ComputeMotionVector(input.positionNDC, input.positionPrev);
	output.motionVector = float4(abs(mv), 0, 1); // 正の値に変換して可視化
	//output.motionVector = float4(1, 0, 0, 1);
	if (textureColor.a <= 0.01f) {
		discard;
	}
	
	if (gMaterial.enableLighting == 0) {
		output.color = gMaterial.color * textureColor;
		if (output.color.a <= 0.01f) {
			discard;
		}
		return output; 
	}
	
	float3 normal = normalize(input.normal);
	float3 pointLightDirection = normalize(input.worldPos.xyz - gPointLight.position);
	float3 toEye = normalize(gDirectionalLight.eyePos - input.worldPos.xyz);
	float3 reflectLight = reflect(normalize(gDirectionalLight.direction), normal);
	float3 halfVector = normalize(-normalize(gDirectionalLight.direction) + toEye);
	
	float3 lightDire = normalize(gDirectionalLight.direction);
	
	float RdotE = dot(reflectLight, toEye);
	float NdotH = dot(normal, halfVector);
	float NdotL = dot(normal, normalize(-gDirectionalLight.direction));
	
	float distance = length(gPointLight.position - input.worldPos.xyz);
	float factor = pow(saturate(-distance / gPointLight.radius + 1.0f), gPointLight.decay);
	
	// ライト空間での座標を計算
	float4 direLightPos = mul(float4(input.worldPos.xyz, 1.0f), gDirectionalLight.lightViewProj);
	direLightPos.xyz /= direLightPos.w;
	float direLightShadow = ShadowBias(direLightPos);
	float visibility = 1.0f - direLightShadow; 
	
	//float4 pointLightPos = mul(float4(input.worldPos.xyz, 1.0f), gPointLight.lightViewProj);
	//pointLightPos.xyz /= pointLightPos.w;
	//float pointLightShadow = ShadowBias(pointLightPos);
	
	//float4 spotLightPos = mul(float4(input.worldPos.xyz, 1.0f), gSpotLight.lightViewProj);
	//spotLightPos.xyz /= spotLightPos.w;
	//float spotLightShadow = ShadowBias(spotLightPos);
	
	// -------------------------------------------------
	// ↓ directional
	// -------------------------------------------------
	float3 directionalLight = DirectionalLighting(NdotL, NdotH, gMaterial.shininess, gDirectionalLight.color.rgb, gMaterial.color.rgb, textureColor.rgb) * (gDirectionalLight.intensity * visibility);
	
	// -------------------------------------------------
	// ↓ point
	// -------------------------------------------------
	float3 phalfVector = normalize(-pointLightDirection + toEye);
	float pNdotH = dot(normalize(input.normal), phalfVector);
	
	float3 pointLight = PointLighting(NdotL, pNdotH, gMaterial.shininess, gPointLight.color.rgb, gMaterial.color.rgb, textureColor.rgb) * gPointLight.intensity;
	
	// -------------------------------------------------
	// ↓ spotLight
	// -------------------------------------------------
	float3 spotLight = SpotLighting(gSpotLight, NdotL, NdotH, input.worldPos.xyz, gMaterial.color.rgb, textureColor.rgb);

	// -------------------------------------------------
	// ↓ limLight
	// -------------------------------------------------
	float lim = 1.0f - saturate(dot(normal, toEye));
	lim *= saturate(1.0f - saturate(dot(normal, lightDire)) + dot(toEye, lightDire));
	float3 limCol = pow(lim, gDirectionalLight.limPower) * gDirectionalLight.color.rgb * textureColor.rgb * gDirectionalLight.intensity;
	
	// -------------------------------------------------
	// ↓ final
	// -------------------------------------------------
	output.color.rgb = directionalLight;
	output.color.rgb += pointLight;
	output.color.rgb += spotLight;
	output.color.rgb += limCol;
	
	output.color.a = gMaterial.color.a * textureColor.a;
	output.color = clamp(output.color, 0.0f, 1.0f);
	
	if (output.color.a <= 0.01f) {
		discard;
	}
	
	return output;
}