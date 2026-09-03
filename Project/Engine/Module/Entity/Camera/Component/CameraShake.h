#pragma once

// engine
#include "Engine/Utilities/Timer.h"
#include "Engine/Lib/Math/Vector3.h"
#include "Engine/Lib/Math/Quaternion.h"
#include "Engine/Module/Entity/Camera/Component/CameraShakeParameters.h"

/// <summary>
/// カメラシェイクを行う際のコンポーネント
/// </summary>
struct CameraShakeResult {
    Math::Vector3 positionOffset{};
    Math::Quaternion rotationOffset{};
};

class CameraShake {
public:
    void Play(const CameraShakeRequest& request);
    void Stop(bool immediate = true);
    void Update(float deltaTime);

    bool IsPlaying() const;
    const CameraShakeResult& GetResult() const;

private:
    CameraShakeResult EvaluateParameter(float time) const;
    CameraShakeResult EvaluateNoise(float time) const;
    float EvaluateDecay() const;

private:
    CameraShakeRequest request_{};
    CameraShakeResult result_{};

    float elapsedTime_ = 0.0f;
    bool isPlaying_ = false;
};
