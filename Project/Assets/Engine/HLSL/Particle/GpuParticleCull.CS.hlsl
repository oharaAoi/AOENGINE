#include "Particle.hlsli"

struct CullingData {
	float4 frustumPlanes[6];
	uint maxParticles;
	float particleLocalRadius;
	uint useFrustumCulling;
	float padding;
};

StructuredBuffer<GpuParticle> gParticles : register(t0);
RWStructuredBuffer<uint> gVisibleParticleIndices : register(u0);
RWByteAddressBuffer gIndirectArgs : register(u1);
ConstantBuffer<CullingData> gCulling : register(b0);

bool IsVisible(float3 center, float radius) {
	if (gCulling.useFrustumCulling == 0) {
		return true;
	}

	for (uint planeIndex = 0; planeIndex < 6; ++planeIndex) {
		float4 plane = gCulling.frustumPlanes[planeIndex];
		if (dot(plane.xyz, center) + plane.w < -radius) {
			return false;
		}
	}

	return true;
}

[numthreads(256, 1, 1)]
void CSmain(uint3 dispatchThreadId : SV_DispatchThreadID) {
	uint particleIndex = dispatchThreadId.x;
	if (particleIndex >= gCulling.maxParticles) {
		return;
	}

	GpuParticle particle = gParticles[particleIndex];
	if (particle.lifeTime <= 0.0f || particle.currentTime >= particle.lifeTime || particle.color.a <= 0.0f) {
		return;
	}

	float maxScale = max(abs(particle.scale.x), max(abs(particle.scale.y), abs(particle.scale.z)));
	if (maxScale <= 0.0f) {
		return;
	}

	float radius = gCulling.particleLocalRadius * maxScale;
	if (!IsVisible(particle.pos, radius)) {
		return;
	}

	// D3D12_DRAW_INDEXED_ARGUMENTSのInstanceCountはbyte offset 4です。
	uint visibleIndex;
	gIndirectArgs.InterlockedAdd(4, 1, visibleIndex);
	gVisibleParticleIndices[visibleIndex] = particleIndex;
}
