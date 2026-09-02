#pragma once
#include <string>
#include "Engine/Module/Entity/Camera/BaseCamera.h"
#include "Engine/System/Editor/Parameter/CustomParameter.h"
#include "Engine/Lib/Math/Vector3.h"

namespace AOENGINE {
	class BaseGameObject;
}

/// <summary>
/// プレイヤーを追従するカメラ
/// </summary>
class FollowCamera : public BaseCamera {
public:

	/// <summary>追従カメラの調整項目</summary>
	struct Parameter : public AOENGINE::CustomParameterSet {
		Math::Vector3 offset = { 0.0f, 3.0f, -25.0f };
		float pitch = 8.0f;
		float smoothTime = 0.2f;
		float maxSpeed = 200.0f;
		bool  followY = true;
		float fixedY = 3.0f;

		// シェイクの揺れ周波数 
		Math::Vector2 shakeFrequency = { 61.7f, 48.3f };

		Parameter() : CustomParameterSet("FollowCamera") {
			AddParameter("Offset", offset, 0.1f);
			AddParameter("Pitch(deg)", pitch, 0.1f, 0.0f, 89.0f);
			AddParameter("Smooth Time", smoothTime, 0.01f, 0.001f, 5.0f);
			AddParameter("Max Speed", maxSpeed, 1.0f, 0.0f, 100000.0f);
			AddParameter("Follow Y", followY);
			AddParameter("Fixed Y", fixedY, 0.1f);
			AddParameter("Shake Frequency", shakeFrequency, 0.1f, 0.0f, 500.0f);
		}
	};

public:

	FollowCamera() = default;
	~FollowCamera() override = default;

	// 初期化、更新
	void Init() override;
	void Update() override;
	// 編集
	void Debug_Gui();

private:

	// 追従対象をシーンから名前で取得する
	void ResolveTarget();

	void FollowTarget(float deltaTime);
	void UpdateShake(float deltaTime);

private:

	// 追従対象
	AOENGINE::BaseGameObject* target_ = nullptr;
	std::string targetName_ = "Player";

	Parameter parameter_;

	// 追従
	Math::Vector3 followVelocity_{};
	Math::Vector3 smoothedTarget_{};
	bool initialized_ = false;

	// シェイク
	float shakeTime_ = 0.0f;
	float shakeTimer_ = 0.0f;
	float shakeStrength_ = 0.0f;
	Math::Vector3 shakeOffset_{};

public: // accessor


	const AOENGINE::BaseGameObject* GetTarget() const { return target_; }
	bool HasTarget() const { return target_ != nullptr; }

	void SetTargetName(const std::string& name) { targetName_ = name; }
	void SetTarget(AOENGINE::BaseGameObject* target) { target_ = target; }
	void SetShake(float time, float strength);

};
