#include "Particle.hlsli"

struct PerFrame {
	float time;
	float deletaTime;
};

RWStructuredBuffer<GpuParticle> gParticles : register(u0);
RWStructuredBuffer<int> gFreeListIndex : register(u1);
RWStructuredBuffer<int> gFreeList : register(u2);
ConstantBuffer<PerFrame> gPerFrame : register(b0);
ConstantBuffer<MaxParticle> gMaxParticles : register(b1);

[numthreads(1024, 1, 1)]
void CSmain(uint3 DTid : SV_DispatchThreadID) {
	int particleIndex = DTid.x;
	if (particleIndex < gMaxParticles.maxParticles) {
		
		gParticles[particleIndex].currentTime += gPerFrame.deletaTime;
		
		//gParticles[particleIndex].acceleration *= gParticles[particleIndex].damping;
		gParticles[particleIndex].acceleration.y = gParticles[particleIndex].gravity;
		gParticles[particleIndex].velocity += gParticles[particleIndex].acceleration * gPerFrame.deletaTime;
		gParticles[particleIndex].velocity *= exp(-gParticles[particleIndex].damping * gPerFrame.deletaTime);
		gParticles[particleIndex].pos += gParticles[particleIndex].velocity * gPerFrame.deletaTime;
		
		float t = gParticles[particleIndex].currentTime / gParticles[particleIndex].lifeTime;
		if (gParticles[particleIndex].lifeOfAlpha == 1) {
			float alpha = 1.0 - t;
			gParticles[particleIndex].color.a = alpha;
		}
		
		if (gParticles[particleIndex].lifeOfScaleUp == 1) {
			gParticles[particleIndex].scale = lerp(float3(0, 0, 0), gParticles[particleIndex].targetScale, t);
		}
		
		if (gParticles[particleIndex].currentTime > gParticles[particleIndex].lifeTime) {
			// スケールに0を入れて出力されないようにする
			gParticles[particleIndex].scale = float3(0.0f, 0.0f, 0.0f);
			gParticles[particleIndex].color.a = 0.0f;
			int freeListIndex;
			InterlockedAdd(gFreeListIndex[0], 1, freeListIndex);
			// 最新のfreeListIndexの場所に死んだparticleのIndexを設定する
			if ((freeListIndex + 1) < gMaxParticles.maxParticles) {
				gFreeList[freeListIndex + 1] = particleIndex;
			}
			else {
				// 本来ここにはこない
				InterlockedAdd(gFreeListIndex[0], -1, freeListIndex);
			}
		}
	}
}