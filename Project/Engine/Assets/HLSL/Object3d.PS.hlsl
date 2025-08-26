#include "Object3d.hlsli"
#include "Light.hlsli"
#include "ShadowMap.hlsli"
#include "Material.hlsli"

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

float2 ComputeMotionVector(float4 currentCS, float4 prevCS) {
    
	float2 currentNDC = currentCS.xy / currentCS.w;
	float2 prevNDC = prevCS.xy / prevCS.w;
	
	float2 currentSS = currentNDC;
	float2 prevSS = prevNDC ;
	
	float2 diff = currentSS - prevSS;
	
	return diff;
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
	if (textureColor.a <= gMaterial.discardValue) {
		discard;
	}
	
	if (gMaterial.enableLighting == 0) {
		output.color = gMaterial.color * textureColor;
		if (output.color.a <= gMaterial.discardValue) {
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
	float direLightShadow = DrawShadow(gShadowMap, gSamplerPoint, direLightPos);
	float visibility = 1.0f - direLightShadow; 
	
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
	float3 spotLight = SpotLighting(gSpotLight, NdotL, NdotH, gMaterial.shininess, input.worldPos.xyz, gMaterial.color.rgb, textureColor.rgb);

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
	
	if (output.color.a <= gMaterial.discardValue) {
		discard;
	}
	
	return output;
}