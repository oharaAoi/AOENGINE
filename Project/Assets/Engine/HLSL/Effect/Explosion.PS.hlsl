// KABOOM-inspired explosion for AOENGINE.
// Based on concepts from Hazel Quantock's 2018 KABOOM shader.
// Original: https://www.shadertoy.com/view/ltGyz1
// CC BY-NC-SA 4.0: https://creativecommons.org/licenses/by-nc-sa/4.0/

#include "../Object3d.hlsli"

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

ConstantBuffer<ExplosionParameters> gExplosionPixel : register(b3);

struct PixelShaderOutput {
    float4 color : SV_TARGET0;
    float4 motionVector : SV_TARGET1;
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
    for (int octave = 0; octave < 5; ++octave) {
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

float ExplosionPattern(float3 p, float blendAmount) {
    float smoothPattern = 1.0f - Fbm(p * 2.0f);
    float largeCells = 1.0f - saturate(VoronoiDistance(p));
    float smallCells = 1.0f - saturate(VoronoiDistance(p * 5.0f));
    float cellularPattern = lerp(largeCells, smallCells, 0.2f);
    return lerp(smoothPattern, cellularPattern, blendAmount);
}

PixelShaderOutput main(VertexShaderOutput input) {
    PixelShaderOutput output = (PixelShaderOutput)0;
    float3 normal = normalize(input.normal);
    float3 viewDirection = normalize(gExplosionPixel.eyePosition - input.worldPos.xyz);
    float time = gExplosionPixel.elapsedTime * gExplosionPixel.animationSpeed;
    float3 noisePosition = normal * gExplosionPixel.noiseScale;
    float warp = Fbm(noisePosition + float3(time * 0.31f, -time * 0.47f, time * 0.19f));
    float3 warpedPosition = noisePosition + warp * gExplosionPixel.turbulence - time * 0.4f;
    float noise = Fbm(warpedPosition);

    if (gExplosionPixel.mode < 0.5f) {
        float fadeIn = smoothstep(0.0f, 0.08f, gExplosionPixel.progress);
        float fadeOut = 1.0f - smoothstep(0.62f, 1.0f, gExplosionPixel.progress);
        float pattern = ExplosionPattern(warpedPosition * 0.55f, 0.7f);
        float crackPattern = ExplosionPattern(warpedPosition * 0.58f + 4.7f, 0.5f);
        float density = saturate(((noise * 0.55f + pattern) * 1.15f - gExplosionPixel.progress * 0.42f) * gExplosionPixel.density);
        float rim = pow(1.0f - saturate(abs(dot(normal, viewDirection))), 2.0f);
        float3 rimColor = lerp(float3(0.3f, 0.04f, 0.0f), gExplosionPixel.innerColor.rgb * 2.5f, rim);
        float3 hotColor = lerp(gExplosionPixel.outerColor.rgb, rimColor, step(0.55f, pattern));
        float3 color = lerp(float3(0.01f, 0.008f, 0.006f), hotColor, step(0.42f, crackPattern));
        color += gExplosionPixel.innerColor.rgb * rim * gExplosionPixel.rimIntensity;
        float alpha = density * fadeIn * fadeOut;
        if (alpha <= 0.01f) discard;
        output.color = float4(color * gExplosionPixel.emissiveIntensity, alpha);
    } else {
        float fresnel = 1.0f - abs(dot(normal, viewDirection));
        float rim = smoothstep(1.0f - gExplosionPixel.shockwaveWidth, 1.0f, fresnel);
        float alpha = rim * gExplosionPixel.alpha * 0.65f;
        if (alpha <= 0.01f) discard;
        output.color = float4(gExplosionPixel.innerColor.rgb * gExplosionPixel.emissiveIntensity, alpha);
    }

    float2 current = input.positionNDC.xy / input.positionNDC.w;
    float2 previous = input.positionPrev.xy / input.positionPrev.w;
    output.motionVector = float4(abs(current - previous), 0.0f, 1.0f);
    return output;
}
