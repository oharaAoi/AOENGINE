#include "../Random.hlsli"
#include "../MatrixMath.hlsli"
#include "Particle.hlsli"

struct Emitter {
	float4 color;
	float3 minScale;
	float3 maxScale;
	float3 targetScale;
	float3 rotate;
	float3 pos;
	float3 prePos;
	float3 size;
	int count;
	int emitType;
	int emitOrigin;
	int lifeOfScaleDown;
	int lifeOfScaleUp;
	int lifeOfAlpha;
	int SeparateByAxisScale;
	float scaleMinScaler;
	float scaleMaxScaler;
	float speed;
	float lifeTime;
	float gravity;
	float damping;
	float radius;
	float angle;
	float height;
	int beAffectedByField;
	int coneEmitFrom;
	float radiusThickness;
	float arc;
	float randomDirectionAmount;
};

struct PerFrame {
	float time;
	float deletaTime;
};

RWStructuredBuffer<GpuParticle> gParticles : register(u0);
RWStructuredBuffer<int> gFreeListIndex : register(u1);
RWStructuredBuffer<int> gFreeList : register(u2);
ConstantBuffer<PerFrame> gPerFrame : register(b0);
ConstantBuffer<MaxParticle> gMaxParticles : register(b1);
ConstantBuffer<Emitter> gEmitter : register(b2);

[numthreads(1, 1, 1)]
void CSmain(uint3 DTid : SV_DispatchThreadID) {
	RandomGenerator generator;
	generator.seed = (DTid + gPerFrame.time) * gPerFrame.time;

	for (int countIndex = 0; countIndex < gEmitter.count; ++countIndex) {
		int freeListIndex;
		InterlockedAdd(gFreeListIndex[0], -1, freeListIndex);
		if (freeListIndex < 0 || freeListIndex >= gMaxParticles.maxParticles) {
			InterlockedAdd(gFreeListIndex[0], 1);
			break;
		}

		const int particleIndex = gFreeList[freeListIndex];
		if (gEmitter.SeparateByAxisScale == 0) {
			float scale = generator.Generated1dRange(gEmitter.scaleMinScaler, gEmitter.scaleMaxScaler);
			gParticles[particleIndex].scale = scale.xxx;
		} else {
			gParticles[particleIndex].scale = generator.Generated3dRangeSize(gEmitter.minScale, gEmitter.maxScale);
		}

		float pathT = gEmitter.count > 1 ? countIndex / float(gEmitter.count - 1) : 0.0f;
		float3 emitterPosition = lerp(gEmitter.prePos, gEmitter.pos, pathT);
		float phi = radians(clamp(gEmitter.arc, 0.0f, 360.0f)) * generator.Generated1dRange(0.0f, 1.0f);
		float coneAngle = radians(clamp(gEmitter.angle, 0.0f, 89.0f));
		float baseRadius = max(gEmitter.radius, 0.0f);
		float coneLength = max(gEmitter.height, 0.0f);
		float coneSlope = tan(coneAngle);
		float axialDistance = 0.0f;
		if (gEmitter.coneEmitFrom == 1 && coneLength > 0.0f) {
			if (coneSlope > 0.0001f) {
				float endRadius = baseRadius + coneSlope * coneLength;
				float volumeRadius = pow(lerp(baseRadius * baseRadius * baseRadius,
					endRadius * endRadius * endRadius, generator.Generated1dRange(0.0f, 1.0f)), 1.0f / 3.0f);
				axialDistance = (volumeRadius - baseRadius) / coneSlope;
			} else {
				axialDistance = generator.Generated1dRange(0.0f, coneLength);
			}
		}
		float sectionRadius = baseRadius + coneSlope * axialDistance;
		float thickness = saturate(gEmitter.radiusThickness);
		float innerRadius = sectionRadius * (1.0f - thickness);
		float radialRandom = generator.Generated1dRange(0.0f, 1.0f);
		float sampledRadius = sqrt(lerp(innerRadius * innerRadius, sectionRadius * sectionRadius, radialRandom));

		float sinPhi;
		float cosPhi;
		sincos(phi, sinPhi, cosPhi);
		float3 localPosition = float3(cosPhi * sampledRadius, axialDistance, sinPhi * sampledRadius);
		float3 localDirection = normalize(float3(
			cosPhi * sin(coneAngle),
			cos(coneAngle),
			sinPhi * sin(coneAngle)));

		float3 randomDirection = generator.Generated3dRange(-1.0f, 1.0f);
		if (dot(randomDirection, randomDirection) > 0.000001f) {
			randomDirection = normalize(randomDirection);
			localDirection = normalize(lerp(localDirection, randomDirection, saturate(gEmitter.randomDirectionAmount)));
		}

		gParticles[particleIndex].rotate = float3(0.0f, 0.0f, 0.0f);
		gParticles[particleIndex].pos = emitterPosition + ApplyEuler(gEmitter.rotate, localPosition);
		gParticles[particleIndex].velocity = ApplyEuler(gEmitter.rotate, localDirection) * gEmitter.speed;
		gParticles[particleIndex].color = gEmitter.color;
		gParticles[particleIndex].targetScale = gEmitter.targetScale;
		gParticles[particleIndex].lifeTime = gEmitter.lifeTime;
		gParticles[particleIndex].currentTime = 0.0f;
		gParticles[particleIndex].acceleration = float3(0.0f, 0.0f, 0.0f);
		gParticles[particleIndex].damping = gEmitter.damping;
		gParticles[particleIndex].gravity = gEmitter.gravity;
		gParticles[particleIndex].lifeOfScaleDown = gEmitter.lifeOfScaleDown;
		gParticles[particleIndex].lifeOfScaleUp = gEmitter.lifeOfScaleUp;
		gParticles[particleIndex].lifeOfAlpha = gEmitter.lifeOfAlpha;
		gParticles[particleIndex].beAffectedByField = gEmitter.beAffectedByField;
	}
}
