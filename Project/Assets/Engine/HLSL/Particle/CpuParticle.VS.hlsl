#include "Particle.hlsli"

struct PerView {
	float4x4 viewProjection;
	float4x4 billboardMat;
};

StructuredBuffer<CpuParticle> gParticles : register(t0);
ConstantBuffer<PerView> gPerView : register(b0);

struct VertexShaderInput {
	float4 position : POSITION0;
	float2 texcoord : TEXCOORD0;
};

VertexShaderOutput main(VertexShaderInput input, uint instanceId : SV_InstanceID) {
	VertexShaderOutput output;
	CpuParticle particle = gParticles[instanceId];

	float4x4 worldMat = particle.worldMat;
	float3 particlePos = worldMat[3].xyz;

	if (particle.isStretch) {
		float3 velocity = particle.velocity;
		float speed = length(velocity);

        // 停止時はそのまま
		if (speed < 0.001f) {
			output.position = mul(input.position, mul(worldMat, gPerView.viewProjection));
		}
		else {
			float3 velocityDir = normalize(velocity);

            // CPUで作ったbillboardの軸を取り出す
			float3 right = worldMat[0].xyz;
			float3 up = worldMat[1].xyz;
			float3 forward = worldMat[2].xyz;
			
			float facing = dot(velocityDir, (particle.cameraPos - particlePos));
			if (facing < 0.0f) {
				right *= -1.0f; // 裏向きのときだけ左右反転してカリング防止
			}

            // 進行方向に沿った"ストレッチ軸"を構築
            // → CPUのforward方向にvelocity方向をブレンド
			float3 stretchDir = normalize(lerp(forward, velocityDir, 0.8f));

            // 伸ばす倍率
			float stretch = 1.0f + speed;
			float halfWidth = 0.5f;

            // UVからローカル座標を求める
			float2 uv = input.texcoord - 0.5f;
			float3 localPos = uv.x * right * halfWidth + uv.y * stretchDir * stretch;
			float3 worldPos = particlePos + localPos;

			output.position = mul(float4(worldPos, 1.0f), gPerView.viewProjection);
		}
	}
	else {
		output.position = mul(input.position, mul(worldMat, gPerView.viewProjection));
	}

	output.texcoord = input.texcoord;
	output.color = particle.color;
	output.discardValue = particle.discardValue;
	output.uvTransform = particle.uvTransform;

	return output;
}