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
		
		// 値のコピー
		GpuParticle particle = gParticles[particleIndex];
		particle.currentTime += dt;
	
		// 加速度の更新
		particle.acceleration.y = particle.gravity;

		// 速度の更新
		float3 velocity = particle.velocity;
		velocity += particle.acceleration * dt;
		
		// 減速の更新
		float damp = 1 / (1 + particle.damping * dt);
		velocity *= damp;
		particle.velocity = velocity;
		
		// 座標の更新
		particle.pos += particle.velocity * dt;
		
		// parameterの更新
		float invLife = rcp(particle.lifeTime);
		float t = particle.currentTime * invLife;
	
		if (particle.lifeOfAlpha == 1) {
			float alpha = 1.0 - t;
			particle.color.a = alpha;
		}
		
		if (particle.lifeOfScaleUp == 1) {
			particle.scale = lerp(float3(0, 0, 0), particle.targetScale, t);
		}
		
		if (particle.currentTime > particle.lifeTime) {
			// スケールに0を入れて出力されないようにする
			particle.scale = float3(0.0f, 0.0f, 0.0f);
			particle.color.a = 0.0f;
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
		
		// 新たに更新する
		gParticles[particleIndex] = particle;
	}
}