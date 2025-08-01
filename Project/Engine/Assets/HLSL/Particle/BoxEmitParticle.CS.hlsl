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
};

struct PerFrame {
	float time;
	float deletaTime;
};

static const float3 corners[8] = {
    float3(-1, -1, -1),
    float3(-1, -1,  1),
    float3(-1,  1, -1),
    float3(-1,  1,  1),
    float3( 1, -1, -1),
    float3( 1, -1,  1),
    float3( 1,  1, -1),
    float3( 1,  1,  1)
};

static const int2 edges[12] = {
	int2(0, 1), int2(0, 2), int2(0, 4),
    int2(1, 3), int2(1, 5), int2(2, 3),
    int2(2, 6), int2(3, 7), int2(4, 5),
    int2(4, 6), int2(5, 7), int2(6, 7)
};

RWStructuredBuffer<GpuParticle> gParticles : register(u0);
RWStructuredBuffer<int> gFreeListIndex : register(u1);
RWStructuredBuffer<int> gFreeList : register(u2);
ConstantBuffer<PerFrame> gPerFrame : register(b0);
ConstantBuffer<MaxParticle> gMaxParticles : register(b1);
ConstantBuffer<Emitter> gEmitter : register(b2);

float3 ApplyVelocityWithRotation(float4 rotation, float3 localVelocity, float threshold) {
    // クォータニオンの長さを計算
	float rotationMagnitude = sqrt(dot(rotation, rotation));
    
    // 閾値を超えない場合、ワールド上方向 (0, 1, 0) を使用
	if (rotationMagnitude < threshold) {
		return float3(0, 1, 0);
	}

    // クォータニオンを正規化
	rotation /= rotationMagnitude;

    // クォータニオンを使ってローカル速度を回転
	float3 u = cross(rotation.xyz, localVelocity);
	float3 v = cross(rotation.xyz, u) + u * rotation.w;

    // ワールド座標系の速度
	return localVelocity + 2.0 * v;
}

[numthreads(1, 1, 1)]
void CSmain(uint3 DTid : SV_DispatchThreadID) {
	RandomGenerator generator;
	generator.seed = (DTid + gPerFrame.time) * gPerFrame.time;
	for (int countIndex = 0; countIndex < gEmitter.count; ++countIndex) {
		int freeListIndex;
			// gFreeCounter[0]に1を足し、足す前の値をparticleIndexに格納
		InterlockedAdd(gFreeListIndex[0], -1, freeListIndex);
			
		if (0 <= freeListIndex && freeListIndex < gMaxParticles.maxParticles) {
			int particleIndex = gFreeList[freeListIndex];
			
			// particleのスケールをわけない場合
			if (gEmitter.SeparateByAxisScale == 0) {
				float scale = generator.Generated1dRange(-gEmitter.scaleMinScaler, gEmitter.scaleMaxScaler);
				gParticles[particleIndex].scale = float3(scale, scale, scale);
			}
			else {
				// particleのスケール軸を分けう時
				float x = generator.Generated1dRange(-gEmitter.minScale.x, gEmitter.maxScale.x);
				float y = generator.Generated1dRange(-gEmitter.minScale.y, gEmitter.maxScale.y);
				float z = generator.Generated1dRange(-gEmitter.minScale.z, gEmitter.maxScale.z);
				gParticles[particleIndex].scale = float3(x, y, z);
			}
			
			// 半径から射出位置を決定する
			float3 emitPos;
			if (gEmitter.emitOrigin == 0) {
				emitPos = gEmitter.pos;
			}
			else if (gEmitter.emitOrigin == 1) {
				float rangeX = generator.Generated1dRange(-gEmitter.size.x, gEmitter.size.x);
				float rangeY = generator.Generated1dRange(-gEmitter.size.y, gEmitter.size.y);
				float rangeZ = generator.Generated1dRange(-gEmitter.size.z, gEmitter.size.z);
				emitPos = float3(rangeX, rangeY, rangeZ) + gEmitter.pos;
			}
			else if (gEmitter.emitOrigin == 2) {
				int edgeIndex = int(floor(generator.Generated1dRange(0, 11)));
				int2 edge = edges[edgeIndex];
				
				float3 localA = corners[edge.x] * (gEmitter.size);
				float3 localB = corners[edge.y] * (gEmitter.size);
				
				float3 worldA = gEmitter.pos + localA;
				float3 worldB = gEmitter.pos + localB;
				
				float t = generator.Generated1dRange(0, 1);
				emitPos = lerp(worldA, worldB, t);
			}
			
			gParticles[particleIndex].rotate = float3(0, 0, 0);
			gParticles[particleIndex].pos = emitPos;
			gParticles[particleIndex].color.rgb = gEmitter.color.rgb;
			gParticles[particleIndex].color.a = gEmitter.color.a;
			gParticles[particleIndex].lifeTime = gEmitter.lifeTime;
			gParticles[particleIndex].currentTime = 0.0f;
			gParticles[particleIndex].acceleration = float3(0, 0, 0);
			gParticles[particleIndex].damping = gEmitter.damping;
			gParticles[particleIndex].gravity = gEmitter.gravity;
			gParticles[particleIndex].lifeOfScaleDown = gEmitter.lifeOfScaleDown;
			gParticles[particleIndex].lifeOfScaleUp = gEmitter.lifeOfScaleDown;
			gParticles[particleIndex].lifeOfAlpha = gEmitter.lifeOfAlpha;
			
			if (gEmitter.emitType == 0) {
				gParticles[particleIndex].velocity = ApplyEuler(gEmitter.rotate, float3(0, 1, 0)) * gEmitter.speed;
				
			}
			else if (gEmitter.emitType == 1) {
				float direX = generator.Generated1dRange(-1, 1);
				float direY = generator.Generated1dRange(-1, 1);
				float direZ = generator.Generated1dRange(-1, 1);
				gParticles[particleIndex].velocity = ApplyEuler(gEmitter.rotate, float3(direX, direY, direZ)) * gEmitter.speed;
				
			}
		}
		else {
				// 発生しなかったので減らした分を元に戻す
			InterlockedAdd(gFreeListIndex[0], 1);
				// これ以上発生しないのでループを抜ける
			break;
		}
	}
}