#include "Object3dInstancing.hlsli"
#include "Light.hlsli"
#include "ShadowMap.hlsli"

// CPU側のNormalInstanceMaterialDataと同じ並びのinstance material。
// TextureだけはSRV heap上のindexとしてalbedoTextureIndexに保持します。
struct InstanceMaterial {
	float4 color;
	int enableLighting;
	uint albedoTextureIndex;
	float2 padding0;
	float4x4 uvTransform;
	float shininess;
	float discardValue;
	float iblScale;
	float padding1;
};

ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);
ConstantBuffer<PointLight> gPointLight : register(b2);
ConstantBuffer<SpotLight> gSpotLight : register(b3);
StructuredBuffer<InstanceMaterial> gInstanceMaterials : register(t0);

// SRV heap全体をTexture配列として参照し、instanceごとに異なるAlbedo Textureを選べるようにします。
Texture2D<float4> gTextures[65536] : register(t0, space1);
Texture2D<float> gShadowMap : register(t1);
TextureCube<float4> gEnviromentTexture : register(t2);
SamplerState gSampler : register(s0);
SamplerComparisonState gShadowSampler : register(s1);

struct PixelShaderOutput {
	float4 color : SV_TARGET0;
	float4 motionVector : SV_TARGET1;
};

float2 ComputeMotionVector(float4 currentCS, float4 prevCS) {
	float2 currentNDC = currentCS.xy / currentCS.w;
	float2 prevNDC = prevCS.xy / prevCS.w;
	return currentNDC - prevNDC;
}

PixelShaderOutput main(VertexShaderOutput input) {
	PixelShaderOutput output = (PixelShaderOutput) 0;
	InstanceMaterial material = gInstanceMaterials[input.instanceId];
	float4 transformedUV = mul(float4(input.texcoord, 0.0f, 1.0f), material.uvTransform);

	// albedoTextureIndexはTextureManagerが割り当てたSRV descriptor indexです。
	float4 textureColor = gTextures[NonUniformResourceIndex(material.albedoTextureIndex)].Sample(gSampler, transformedUV.xy);
	
	float2 mv = ComputeMotionVector(input.positionNDC, input.positionPrev);
	output.motionVector = float4(abs(mv), 0, 1);
	if (textureColor.a <= material.discardValue) {
		discard;
	}
	
	if (material.enableLighting == 0) {
		output.color = material.color * textureColor;
		if (output.color.a <= material.discardValue) {
			discard;
		}
		return output; 
	}

	float3 normal = normalize(input.normal);
	float3 pointLightDirection = normalize(input.worldPos.xyz - gPointLight.position);
	float3 toEye = normalize(gDirectionalLight.eyePos - input.worldPos.xyz);
	float3 reflectLight = reflect(normalize(gDirectionalLight.direction), normal);
	float3 halfVector = normalize(-normalize(gDirectionalLight.direction) + toEye);
	
	float3 L = normalize(-gDirectionalLight.direction);
	float3 V = normalize(toEye);
	float3 N = normalize(normal);

	float3 H = normalize(L + V);
	float3 R = reflect(-L, N);

	float NdotL = saturate(dot(N, L));
	float NdotH = saturate(dot(N, H));
	float RdotV = saturate(dot(R, V));
	float RdotE = dot(reflectLight, toEye);
	
	float3 lightDire = normalize(-gDirectionalLight.direction);
	
	float distance = length(gPointLight.position - input.worldPos.xyz);
	float factor = pow(saturate(-distance / gPointLight.radius + 1.0f), gPointLight.decay);
	
	float4 lightClip = mul(float4(input.worldPos.xyz, 1.0f), gDirectionalLight.lightViewProj);
	float direLightShadow = DrawShadow(gShadowMap, gShadowSampler, lightClip, gDirectionalLight.direction, normal);
	float visibility = direLightShadow; 
	
	float3 directionalLight = DirectionalLighting(NdotL, NdotH, material.shininess, gDirectionalLight.color.rgb, material.color.rgb, textureColor.rgb) * (gDirectionalLight.intensity * visibility);
	
	float3 phalfVector = normalize(-pointLightDirection + toEye);
	float pNdotH = dot(normalize(input.normal), phalfVector);
	float3 pointLight = PointLighting(NdotL, pNdotH, material.shininess, gPointLight.color.rgb, material.color.rgb, textureColor.rgb) * gPointLight.intensity;
	
	float3 spotLight = SpotLighting(gSpotLight, NdotL, NdotH, material.shininess, input.worldPos.xyz, material.color.rgb, textureColor.rgb);

	float lim = 1.0f - saturate(dot(normal, toEye));
	lim *= saturate(1.0f - saturate(dot(normal, lightDire)) + dot(toEye, lightDire));
	float3 limCol = pow(lim, gDirectionalLight.limPower) * gDirectionalLight.color.rgb * textureColor.rgb * gDirectionalLight.intensity;
	
	float3 cameraToPos = normalize(input.worldPos.xyz - gDirectionalLight.eyePos);
	float3 reflectVector = reflect(cameraToPos, normalize(input.normal));
	float4 environmentColor = gEnviromentTexture.Sample(gSampler, reflectVector);
	
	output.color.rgb = directionalLight;
	output.color.rgb += pointLight;
	output.color.rgb += spotLight;
	output.color.rgb += limCol;
	output.color.rgb += environmentColor.rgb * material.iblScale;
	
	output.color.a = material.color.a * textureColor.a;
	output.color = clamp(output.color, 0.0f, 1.0f);
	
	if (output.color.a <= material.discardValue) {
		discard;
	}
	
	return output;
}
