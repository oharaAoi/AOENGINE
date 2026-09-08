// KABOOM-inspired explosion for AOENGINE.
// Based on concepts from Hazel Quantock's 2018 KABOOM shader.
// Original: https://www.shadertoy.com/view/ltGyz1
// CC BY-NC-SA 4.0: https://creativecommons.org/licenses/by-nc-sa/4.0/

#include "../Object3d.hlsli"

struct WorldTransformMatrix {
    float4x4 world;
    float4x4 worldPerv;
    float4x4 worldInverseTranspose;
};

struct ViewProjectionMatrix {
    float4x4 view;
    float4x4 projection;
};

struct ExplosionParameters {
    float progress;
    float elapsedTime;
    float noiseStrength;
    float noiseScale;
    float4 innerColor;
    float4 outerColor;
    float3 eyePosition;
    float mode;
    float emissiveIntensity;
    float shockwaveWidth;
    float alpha;
    float animationSpeed;
    float turbulence;
    float density;
    float rimIntensity;
    float padding;
};

ConstantBuffer<WorldTransformMatrix> gWorldTransformMatrix : register(b0);
ConstantBuffer<ViewProjectionMatrix> gViewProjectionMatrix : register(b1);
ConstantBuffer<ViewProjectionMatrix> gViewProjectionMatrixPrev : register(b2);
ConstantBuffer<ExplosionParameters> gExplosionVertex : register(b3);

struct VertexShaderInput {
    float4 position : POSITION0;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
    float4 tangent : TANGENT0;
};

float Hash31(float3 p) {
    return frac(sin(dot(p, float3(127.1f, 311.7f, 74.7f))) * 43758.5453f);
}

float ValueNoise(float3 p) {
    float3 cell = floor(p);
    float3 f = frac(p);
    f = f * f * (3.0f - 2.0f * f);
    float n000 = Hash31(cell + float3(0, 0, 0));
    float n100 = Hash31(cell + float3(1, 0, 0));
    float n010 = Hash31(cell + float3(0, 1, 0));
    float n110 = Hash31(cell + float3(1, 1, 0));
    float n001 = Hash31(cell + float3(0, 0, 1));
    float n101 = Hash31(cell + float3(1, 0, 1));
    float n011 = Hash31(cell + float3(0, 1, 1));
    float n111 = Hash31(cell + float3(1, 1, 1));
    return lerp(lerp(lerp(n000, n100, f.x), lerp(n010, n110, f.x), f.y),
                lerp(lerp(n001, n101, f.x), lerp(n011, n111, f.x), f.y), f.z);
}

float Fbm(float3 p) {
    float value = 0.0f;
    float amplitude = 0.5f;
    [unroll]
    for (int octave = 0; octave < 4; ++octave) {
        value += ValueNoise(p) * amplitude;
        p = p * 2.03f + float3(17.1f, 9.2f, 13.7f);
        amplitude *= 0.5f;
    }
    return value;
}

float3 Hash33(float3 p) {
    return frac(sin(float3(dot(p, float3(127.1f, 311.7f, 74.7f)),
                           dot(p, float3(269.5f, 183.3f, 246.1f)),
                           dot(p, float3(113.5f, 271.9f, 124.6f)))) * 43758.5453f);
}

float VoronoiDistance(float3 p) {
    float3 cell = floor(p);
    float3 local = frac(p);
    float closest = 10.0f;
    [unroll]
    for (int z = -1; z <= 1; ++z) {
        [unroll]
        for (int y = -1; y <= 1; ++y) {
            [unroll]
            for (int x = -1; x <= 1; ++x) {
                float3 offset = float3(x, y, z);
                float3 node = offset + (Hash33(cell + offset) - 0.5f) * 0.7f;
                closest = min(closest, length(local - node));
            }
        }
    }
    return closest * closest;
}

VertexShaderOutput main(VertexShaderInput input) {
    VertexShaderOutput output;
    float4 localPosition = input.position;
    if (gExplosionVertex.mode < 0.5f) {
        float time = gExplosionVertex.elapsedTime * gExplosionVertex.animationSpeed;
        float3 noisePosition = input.normal * gExplosionVertex.noiseScale;
        float warp = Fbm(noisePosition + float3(time * 0.37f, -time * 0.51f, time * 0.23f));
        float3 warpedPosition = noisePosition + warp * gExplosionVertex.turbulence - time * 0.35f;
        float smoothNoise = Fbm(warpedPosition);
        float cellular = 1.0f - saturate(VoronoiDistance(warpedPosition * 0.55f));
        float noise = lerp(smoothNoise, cellular, 0.72f);
        float envelope = sin(gExplosionVertex.progress * 3.14159265f);
        localPosition.xyz += input.normal * ((noise - 0.45f) * 2.0f * gExplosionVertex.noiseStrength * envelope);
    }

    float4x4 wvp = mul(gWorldTransformMatrix.world, mul(gViewProjectionMatrix.view, gViewProjectionMatrix.projection));
    float4x4 previousWvp = mul(gWorldTransformMatrix.worldPerv, mul(gViewProjectionMatrixPrev.view, gViewProjectionMatrixPrev.projection));
    output.position = mul(localPosition, wvp);
    output.positionNDC = output.position;
    output.positionPrev = mul(input.position, previousWvp);
    output.texcoord = input.texcoord;
    output.normal = normalize(mul(input.normal, (float3x3)gWorldTransformMatrix.worldInverseTranspose));
    output.worldPos = mul(localPosition, gWorldTransformMatrix.world);
    return output;
}
