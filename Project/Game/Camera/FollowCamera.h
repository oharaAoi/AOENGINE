#pragma once
#include <string>
#include "Engine/Module/Entity/Camera/BaseCamera.h"
#include "Engine/Lib/Json/IJsonConverter.h"
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

	/// <summary>
	/// 追従カメラの調整項目
	/// </summary>
	struct Parameter :
		public AOENGINE::CustomParameterSet,
		public AOENGINE::IJsonConverter {
		Math::Vector3 offset;			// 追従対象からの相対位置
		float pitch;					// 見下ろし角
		float smoothTime;				// スクロールのイージング
		float maxSpeed;					// スクロールの最大速度
		float scrollHeight;				// 1回のスクロールで上昇する高さ
		Math::Vector2 shakeFrequency;	// シェイクの揺れ周波数

		Parameter() : CustomParameterSet("FollowCamera") {
			SetGroupName("Camera");
			SetName("followCamera");

			AddParameter("Offset", offset, 0.1f);
			AddParameter("Pitch(deg)", pitch, 0.1f, 0.0f, 89.0f);
			AddParameter("Scroll Smooth Time", smoothTime, 0.01f, 0.001f, 5.0f);
			AddParameter("Scroll Max Speed", maxSpeed, 1.0f, 0.0f, 100000.0f);
			AddParameter("Scroll Height", scrollHeight, 0.1f, 0.0f, 100000.0f);
			AddParameter("Shake Frequency", shakeFrequency, 0.1f, 0.0f, 500.0f);
		}

		json ToJson(const std::string& id) const override {
			return AOENGINE::JsonBuilder(id)
				.Add("offset", offset)
				.Add("pitch", pitch)
				.Add("smoothTime", smoothTime)
				.Add("maxSpeed", maxSpeed)
				.Add("scrollHeight", scrollHeight)
				.Add("shakeFrequency", shakeFrequency)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			Convert::fromJson(jsonData, "offset", offset);
			Convert::fromJson(jsonData, "pitch", pitch);
			Convert::fromJson(jsonData, "smoothTime", smoothTime);
			Convert::fromJson(jsonData, "maxSpeed", maxSpeed);
			Convert::fromJson(jsonData, "scrollHeight", scrollHeight);
			Convert::fromJson(jsonData, "shakeFrequency", shakeFrequency);
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

	// 段階スクロール
	float cameraTargetY_ = 0.0f;
	bool isScrolling_ = false;
	static constexpr float kScrollArriveThreshold = 0.01f;

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
