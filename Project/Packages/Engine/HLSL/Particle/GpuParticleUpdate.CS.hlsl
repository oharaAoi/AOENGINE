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

[numthreads(256, 1, 1)]
void CSmain(uint3 DTid : SV_DispatchThreadID) {
	int particleIndex = DTid.x;
	if (DTid.x >= gMaxParticles.maxParticles) {
		return;
	}
	
	if (particleIndex < gMaxParticles.maxParticles) {
		// デルタタイムの宣言
		float dt = gPerFrame.deletaTime;
		gParticles[particleIndex].currentTime += dt;
	
		// 加速度の更新
		gParticles[particleIndex].acceleration.y = gParticles[particleIndex].gravity;

		// 速度の更新
		float3 velocity = gParticles[particleIndex].velocity;
		velocity += gParticles[particleIndex].acceleration * dt;
		
		// 減速の更新
		float damp = 1 / (1 + gParticles[particleIndex].damping * dt);
		velocity *= damp;
		gParticles[particleIndex].velocity = velocity;
		
		// 座標の更新
		gParticles[particleIndex].pos += gParticles[particleIndex].velocity * dt;
		
		// parameterの更新
		float invLife = rcp(gParticles[particleIndex].lifeTime);
		float t = gParticles[particleIndex].currentTime * invLife;
	
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