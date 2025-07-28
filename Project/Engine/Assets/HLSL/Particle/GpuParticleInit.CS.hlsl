#include "Particle.hlsli"

RWStructuredBuffer<GpuParticle> gParticles : register(u0);
RWStructuredBuffer<int> gFreeListIndex : register(u1);
RWStructuredBuffer<uint> gFreeList : register(u2);
ConstantBuffer<MaxParticle> gMaxParticles : register(b0);

[numthreads(1024, 1, 1)]
void CSmain(uint3 DTid : SV_DispatchThreadID) {
	int countIndex = DTid.x;
	
	if (countIndex == 0) {
		gFreeListIndex[0] = gMaxParticles.maxParticles - 1;
		gFreeList[0] = 0;
	}
	
	if (countIndex < gMaxParticles.maxParticles) {
		gParticles[countIndex] = (GpuParticle) 0;
		gFreeList[countIndex] = countIndex;

	}
}