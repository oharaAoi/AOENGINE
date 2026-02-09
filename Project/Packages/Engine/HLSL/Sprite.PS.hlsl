#include "Sprite.hlsli"

static const float PI = 3.141592653589f;

struct Material {
	float4 color;
	float4x4 uvTransform;
	float2 uvMin;
	float2 uvMax;
	int arcType;
};

struct ArcGaugeParam {
	float2 center; // 中心座標
	float fillAmount; // 塗りつぶし量
	float innerRadius; // 内半径
	float outerRadius; // 外半径
	float startAngle; // 開始角度
	float arcRange; // 弧の最大角度
	int clockwise; // 回転方向
};

ConstantBuffer<Material> gMaterial : register(b0);
ConstantBuffer<ArcGaugeParam> gArcParam : register(b1);
Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct PixelShaderOutput {
	float4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input) {
	PixelShaderOutput output;
	float2 uv = input.texcoord;
	float4 transformedUV = mul(float4(uv, 0.0f, 1.0f), gMaterial.uvTransform);
	float4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
	
	if (textureColor.a <= 0.01f) {
		discard;
	}
	
	if (gMaterial.color.a <= 0.01f) {
		discard;
	}
	
	if (transformedUV.x < gMaterial.uvMin.x) {
		discard;
	}
	
	if (transformedUV.y < gMaterial.uvMin.y) {
		discard;
	}
	
	if (transformedUV.x > gMaterial.uvMax.x) {
		discard;
	}
	
	if (transformedUV.y > gMaterial.uvMax.y) {
		discard;
	}
	
	if (gMaterial.arcType == 1) {
		// 中心からのベクトル
		float2 dir = uv - gArcParam.center;
		float dist = length(dir);
		
		// 角度
		float angle = atan2(dir.y, dir.x);
		angle = (angle < 0) ? angle + PI * 2.0f : angle;
		
		// startAngle基準に補正
		float relAngle = angle - gArcParam.startAngle;
		if (relAngle < 0) {
			relAngle += PI * 2.0f;
		}
		
		float maxFillAngle = gArcParam.arcRange * gArcParam.fillAmount;
		
		// 終了角度
		float endAngle = gArcParam.startAngle + gArcParam.arcRange * gArcParam.fillAmount;
		
		// 半径と角度の判定
		bool insideRadius = (dist >= gArcParam.innerRadius && dist <= gArcParam.outerRadius);
		// 時計回り or 反時計回り
		bool insideAngle;
		if (gArcParam.clockwise == 1) {
			// 時計回り
			insideAngle = (relAngle >= (gArcParam.arcRange - maxFillAngle) && relAngle <= gArcParam.arcRange);
		} else {
			// 反時計回り
			insideAngle = (relAngle <= maxFillAngle);
		}

		if (!insideRadius || !insideAngle) {
			textureColor = float4(0, 0, 0, 0);
		}
	}
	
	output.color = textureColor * gMaterial.color;
	output.color = clamp(output.color, 0.0f, 1.0f);
	output.color.a = gMaterial.color.a * textureColor.a;
	
	return output;
}