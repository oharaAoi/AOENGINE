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
		Math::Vector3 offset;			  // 追従対象からの相対位置
		float pitch;					  // 見下ろし角
		float smoothTime;				  // スクロールのイージング
		float maxSpeed;					  // スクロールの最大速度
		float scrollHeight;				  // 1回のスクロールで上昇する高さ
		float scrollTriggerScreenY = 0.5f;// スクロールを始めるプレイヤーの画面上の高さ
		// スクロール位置を示す線を、その高さから何ピクセルずらして置くか
		float scrollLineOffsetY = 0.0f;

		Parameter() : CustomParameterSet("FollowCamera") {
			SetGroupName("Camera");
			SetName("followCamera");

			AddParameter("Offset", offset, 0.1f);
			AddParameter("Pitch(deg)", pitch, 0.1f, 0.0f, 89.0f);
			AddParameter("Scroll Smooth Time", smoothTime, 0.01f, 0.001f, 5.0f);
			AddParameter("Scroll Max Speed", maxSpeed, 1.0f, 0.0f, 100000.0f);
			AddParameter("Scroll Height", scrollHeight, 0.1f, 0.0f, 100000.0f);
			AddParameter("Scroll Trigger Screen Y", scrollTriggerScreenY, 0.01f, 0.0f, 1.0f);
			AddParameter("Scroll Line Offset Y(px)", scrollLineOffsetY, 1.0f, -2000.0f, 2000.0f);
		}

		json ToJson(const std::string& id) const override {
			return AOENGINE::JsonBuilder(id)
				.Add("offset", offset)
				.Add("pitch", pitch)
				.Add("smoothTime", smoothTime)
				.Add("maxSpeed", maxSpeed)
				.Add("scrollHeight", scrollHeight)
				.Add("scrollTriggerScreenY", scrollTriggerScreenY)
				.Add("scrollLineOffsetY", scrollLineOffsetY)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			Convert::fromJson(jsonData, "offset", offset);
			Convert::fromJson(jsonData, "pitch", pitch);
			Convert::fromJson(jsonData, "smoothTime", smoothTime);
			Convert::fromJson(jsonData, "maxSpeed", maxSpeed);
			Convert::fromJson(jsonData, "scrollHeight", scrollHeight);
			Convert::fromJson(jsonData, "scrollTriggerScreenY", scrollTriggerScreenY);
			Convert::fromJson(jsonData, "scrollLineOffsetY", scrollLineOffsetY);
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

	/// <summary>
	/// プレイヤーが決めた画面高さまで来たら、1段ぶん上へスクロールする
	/// </summary>
	void UpdateStepScroll(const Math::Vector3& targetPos, float deltaTime);

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

	// 演出で一時的に足す位置。フェーズ切り替えで寄せるのに使う
	Math::Vector3 extraOffset_{};

	// 追従を止めているか。演出中にスクロールが走らないようにする
	bool isFollowPaused_ = false;

	// カメラシェイクの調整用リクエスト
	CameraShakeRequest shakeRequest_;

public: // accessor


	const AOENGINE::BaseGameObject* GetTarget() const { return target_; }
	bool HasTarget() const { return target_ != nullptr; }

	/// <summary>スクロールの目印を置くための参照。判定と同じ値を使う</summary>
	float GetScrollTriggerScreenY() const { return parameter_.scrollTriggerScreenY; }
	float GetScrollLineOffsetY() const { return parameter_.scrollLineOffsetY; }

	void SetTargetName(const std::string& name) { targetName_ = name; }
	void SetTarget(AOENGINE::BaseGameObject* target) { target_ = target; }

	/// <summary>演出で一時的にカメラをずらす</summary>
	void SetExtraOffset(const Math::Vector3& offset) { extraOffset_ = offset; }
	const Math::Vector3& GetExtraOffset() const { return extraOffset_; }

	/// <summary>
	/// 追従を止める。
	/// </summary>
	void SetFollowPaused(bool isPaused) { isFollowPaused_ = isPaused; }

};
