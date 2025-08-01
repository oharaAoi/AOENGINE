#include "Particle.hlsli"

struct AccelerationField {
	float3 acceleration;
	float3 min;
	float3 max;
};

struct PerFrame {
	float time;
	float deletaTime;
};

RWStructuredBuffer<GpuParticle> gParticles : register(u0);
ConstantBuffer<MaxParticle> gMaxParticles : register(b0);
ConstantBuffer<PerFrame> gPerFrame : register(b1);
ConstantBuffer<AccelerationField> gAccelerationField : register(b2);

bool IsCollision(float3 min, float3 max, float3 pos) {
	   // 最近傍点を求める
	float3 closestPoint = {
		clamp(pos.x, min.x, max.x),
		clamp(pos.y, min.y, max.y),
		clamp(pos.z, min.z, max.z)
	};

	// 最近傍点ト球の中心の距離を求める
	float distance = length(closestPoint - pos);

	// 距離が半径よりも小さければ衝突
	if (distance <= 1.0f) {
		return true;
	}

	return false;
}

[numthreads(256, 1, 1)]
void CSmain(uint3 DTid : SV_DispatchThreadID) {
	int particleIndex = DTid.x;
	if (particleIndex < gMaxParticles.maxParticles) {
		if (IsCollision(gAccelerationField.min, gAccelerationField.max, gParticles[particleIndex].pos)) {
			gParticles[particleIndex].acceleration = gAccelerationField.acceleration;
		}
	}
}