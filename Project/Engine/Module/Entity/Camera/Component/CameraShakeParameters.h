#pragma once

// stl
#include <cstdint>

#define NOMINMAX
#define _USE_MATH_DEFINES

// engine
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Engine/Lib/Math/Vector3.h"

enum class CameraShakeSource {
    Parameter, // 周波数などを直接指定した周期的な揺れ
    Noise      // ノイズをサンプリングした不規則な揺れ
};

enum class CameraShakeNoiseType {
    White,     // 衝撃的・荒い
    Value,     // 滑らかなランダム
    Perlin     // 自然で連続的
};

enum class CameraShakeDecay {
    None,
    Linear,
    EaseOut
};

struct CameraShakeRequest : public AOENGINE::IJsonConverter {
	CameraShakeRequest();

	json ToJson(const std::string& id) const override;
	void FromJson(const json& jsonData) override;

    CameraShakeSource source = CameraShakeSource::Parameter;

    float duration = 0.3f;

    // 共通倍率。実行時に揺れ全体を調整する値
    float strength = 1.0f;

    // strength = 1.0 のときの最大移動量
    Math::Vector3 positionAmplitude = { 0.3f, 0.3f, 0.0f };

    // オイラー角。単位は度などに統一
    Math::Vector3 rotationAmplitude = { 0.0f, 0.0f, 1.0f };

    CameraShakeDecay decay = CameraShakeDecay::EaseOut;

    // Parameterモード
    Math::Vector3 positionFrequency = { 25.0f, 31.0f, 0.0f };
    Math::Vector3 rotationFrequency = { 0.0f, 0.0f, 20.0f };
    Math::Vector3 phase = { 0.0f, 1.7f, 3.1f };

    // Noiseモード
    CameraShakeNoiseType noiseType = CameraShakeNoiseType::Perlin;
    float noiseStrength = 1.0f;
    float noiseFrequency = 15.0f;
    uint32_t seed = 0;

    void Debug_Gui();
};
