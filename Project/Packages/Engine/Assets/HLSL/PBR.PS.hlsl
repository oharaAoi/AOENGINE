#include "PBR.hlsli"
#include "Light.hlsli"
#include "ShadowMap.hlsli"

// 定数定義
static const float PI = 3.141592653589f;
#define EPSILON 1e-6

struct Material {
	float4 color;
	int enableLighting;
	float4x4 uvTransform;
	float4 diffuseColor; // 色
	float4 specularColor;
	float roughness; // 粗さ
	float metallic; // 金属度
	float shininess; // 鋭さ
	float ambientIntensity; // 環境光の強さ
};

ConstantBuffer<Material> gMaterial : register(b0);
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);
ConstantBuffer<PointLight> gPointLight : register(b2);
ConstantBuffer<SpotLight> gSpotLight : register(b3);
Texture2D<float4> gTexture : register(t0);
Texture2D<float> gShadowMap : register(t1);
TextureCube<float4> gEnviromentTexture : register(t2);
//Texture2D<float3> gNormapMap : register(t2);
//Texture2D<float> gMetallicMap : register(t2);
//Texture2D<float> gRoughnessMap : register(t3);
SamplerState gSampler : register(s0);
SamplerComparisonState gShadowSampler : register(s1);

struct PixelShaderOutput {
	float4 color : SV_TARGET0;
	float4 motionVector : SV_TARGET1;
};

float2 ComputeMotionVector(float4 currentCS, float4 prevCS) {
    
	float2 currentNDC = currentCS.xy / currentCS.w;
	float2 prevNDC = prevCS.xy / prevCS.w;
	
	float2 currentSS = currentNDC;
	float2 prevSS = prevNDC;
	
	float2 diff = currentSS - prevSS;
	
	return diff;
}

//////////////////////////////////////////////////////////////
// 関数
//////////////////////////////////////////////////////////////

//==========================================
// Fresnel(Schlick)		F
//==========================================
float4 SchlickFresnel(float VDotH, float4 ks) {
	float4 fresnel = (ks + (1.0f - ks) * pow((1.0f - VDotH), 5.0f));
	return fresnel;
}

//==========================================
// GGX					D
//==========================================
float GGX(float NDotH, float roughness) {
	float a = roughness * roughness;
	float a2 = a * a;
	float denom = (NDotH * NDotH) * (a2 - 1.0) + 1.0;
	return a2 / (PI * denom * denom);
}

//==========================================
// Height Correlated Smith		G
//==========================================
float HCSmith(float NdotV, float NdotL, float roughness) {
	float NdotV2 = NdotV * NdotV;
	float NdotL2 = NdotL * NdotL;
	
	float Lambda_v = (-1.0 + sqrt(roughness * (1.0 - NdotV2) / NdotV2 + 1.0)) * 0.5f;
	float Lambda_l = (-1.0 + sqrt(roughness * (1.0 - NdotL2) / NdotL2 + 1.0)) * 0.5f;
	return 1.0 / (1.0 + Lambda_l + Lambda_v);
}

//==========================================
// BRDF(双方向反射率分布関数)
//==========================================
float4 BRDF(float NdotH, float NDotV, float NDotL, float VDotH, float4 ks, float roughness) {
	float D = GGX(NdotH, roughness);
	float G = HCSmith(NDotV, NDotL, roughness);
	float4 F = SchlickFresnel(VDotH, ks);
	
	float denom = max(4.0 * NDotV * NDotL, EPSILON);
	float4 brdf = (D * G * F) / denom;
	
	brdf = saturate(brdf);
	
	return brdf;
}

float3 EvalLight(float3 N, float3 V, float3 L, float3 lightColor, float intensity, float3 baseColor, float metallic, float roughness, float visibility) {
	float3 H = normalize(V + L);

	float NdotL = saturate(dot(N, L));
	float NdotV = saturate(dot(N, V));
	float NdotH = saturate(dot(N, H));
	float VdotH = saturate(dot(V, H));

	if (NdotL <= 0.0)
		return 0.0;

	float3 F0 = lerp(float3(0.04, 0.04, 0.04), baseColor, metallic);
	float3 ks = F0;
	float3 kd = (1.0 - ks) * (1.0 - metallic);

	float3 diffuse = kd * baseColor / PI;
	float3 specular =
        BRDF(NdotH, NdotV, NdotL, VdotH, float4(F0, 1), roughness).rgb;

	return (diffuse + specular)
        * NdotL
        * intensity
        * lightColor
        * visibility;
}

//////////////////////////////////////////////////////////////
// main
//////////////////////////////////////////////////////////////
PixelShaderOutput main(VertexShaderOutput input) {
	PixelShaderOutput output;
	
	float2 mv = ComputeMotionVector(input.positionNDC, input.positionPrev);
	output.motionVector = float4(abs(mv), 0, 1); // 正の値に変換して可視化
	
	//=======================================================
	// TextureのUVを求める
	//=======================================================
	float4 transformedUV = mul(float4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
	float4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
	float3 baseColor = textureColor.rgb * gMaterial.color.rgb;
	
	//=======================================================
	// Test
	//=======================================================
	float3 N = normalize(input.normal);
	float3 V = normalize(gDirectionalLight.eyePos - input.worldPos.xyz);
	float NdotV = saturate(dot(N, V));

	float perceptualRoughness = saturate(gMaterial.roughness);
	float roughness = max(perceptualRoughness * perceptualRoughness, EPSILON);
	float metallic = saturate(gMaterial.metallic);

	float3 F0 = lerp(float3(0.04, 0.04, 0.04), baseColor, metallic);
	float3 kd = (1.0 - F0) * (1.0 - metallic);
	float3 R = reflect(-V, N);

	// Mip数は EnvCube の mipCount-1（仮でOK）
	float mipCount = 5.0;
	float mipLevel = perceptualRoughness * mipCount;
	
	//=======================================================
	// 影を計算
	//=======================================================
	float3 lightDir = normalize(-gDirectionalLight.direction);
	
	// shadowMapから影の部分を抽出
	float4 lightClip = mul(float4(input.worldPos.xyz, 1.0f), gDirectionalLight.lightViewProj);
	float direLightShadow = DrawShadow(gShadowMap, gShadowSampler, lightClip, lightDir, N);
	float visibility = direLightShadow;
	

	float3 specularIBL =
    gEnviromentTexture.SampleLevel(
        gSampler,
        R,
        mipLevel
    ).rgb;

	// Fresnel で金属感を反映
	float3 F = SchlickFresnel(NdotV, float4(F0, 1)).rgb;
	specularIBL *= F;
	
	float3 diffuseIBL =
    gEnviromentTexture.SampleLevel(
        gSampler,
        N,
        mipCount // かなりぼかす
    ).rgb
    * baseColor
    * kd;
	
	//=======================================================
	// 法線マップのタイリングを計算する
	//=======================================================
	float tilingFactor = 20.0; // タイリングを小さくするためのスケール（2.0でテクスチャが2倍のサイズに見える）
	float2 scaledUV = transformedUV.xy * tilingFactor; // UV座標をスケーリング
	//float3 normalMap = gNormapMap.Sample(gSampler, scaledUV).xyz * 2.0 - 1.0;
	//float3 normal = normalize(mul(normalMap, input.tangentMat));
	
	//=======================================================
	// ラフネスとメタリックのmapを計算する
	//=======================================================
	//float metallicMap = gMetallicMap.Sample(gSampler, transformedUV.xy).r;
	//float roughnessMap = gRoughnessMap.Sample(gSampler, transformedUV.xy).r;

	//float roughness = roughnessMap * roughnessMap + EPSILON;
	//float metallic = metallicMap;
	
	//float roughness = gMaterial.roughness * gMaterial.roughness + EPSILON;
	//float metallic = gMaterial.metallic;
	
	//=======================================================
	// DirectionalLight
	//=======================================================

	float3 colorDir = EvalLight(
        N, V, lightDir,
        gDirectionalLight.color.rgb,
        gDirectionalLight.intensity,
        textureColor.rgb,
        metallic,
        roughness,
        visibility
    );
	
	//=======================================================
	// PointLight
	//=======================================================
	float3 toPoint = gPointLight.position - input.worldPos.xyz;
	float dist = length(toPoint);
	float3 pointL = toPoint / dist;

	// 距離減衰（例）
	float attenuation =
    saturate(1.0 - dist / gPointLight.radius);
	attenuation *= attenuation;

	float3 colorPoint = EvalLight(
        N, V, pointL,
        gPointLight.color.rgb,
        gPointLight.intensity * attenuation,
        textureColor.rgb,
        metallic,
        roughness,
        1.0 // Point Light は今は影なし
    );
	
	//=======================================================
	// SpotLight
	//=======================================================
	
	float3 toSpot = gSpotLight.position - input.worldPos.xyz;
	float distS = length(toSpot);
	float3 spotL = toSpot / distS;

	// 距離減衰
	float attenS =
    saturate(1.0 - distS / gSpotLight.cosAngle);
	attenS *= attenS;

	// スポット角
	float cosTheta = dot(spotL, normalize(-gSpotLight.direction));
	float spotFactor = saturate((cosTheta - gSpotLight.distance) / (gSpotLight.cosFalloffStart - gSpotLight.cosAngle));

	float3 colorSpot = EvalLight(
        N, V, spotL,
        gSpotLight.color.rgb,
        gSpotLight.intensity * attenS * spotFactor,
        textureColor.rgb,
        metallic,
        roughness,
        1.0
    );
	
	//=======================================================
	// IBL
	//=======================================================

	float3 ambientIBL = (diffuseIBL + specularIBL) * gMaterial.ambientIntensity;
	
	//=======================================================
	 // 反射と拡散のバランスを取る
	float3 direct = colorDir + colorPoint + colorSpot;
	float3 finalRGB = direct + ambientIBL;

	output.color.rgb = finalRGB;
	output.color.a = gMaterial.color.a * textureColor.a;
	output.color = clamp(output.color, 0.0f, 1.0f);
	
	if (output.color.a <= 0.0f) {
		discard;
	}

	return output;
}