#pragma once
#include "Engine/Module/Components/Attribute/AttributeGui.h"
#include "Engine/Module/PostEffect/Grayscale.h"
#include "Engine/Module/PostEffect/Vignette.h"
#include "Game/Camera/BaseCamera.h"
#include "Game/UI/Reticle.h"
#include <utility>

class Player;

/// <summary>
/// Playerに追従するCamera
/// </summary>
class FollowCamera :
	public BaseCamera,
	public AttributeGui {
public:

	struct CameraParameter : public IJsonConverter {
		float distance = 20.0f;	// カメラの距離
		float rotateDelta = 0.04f;
		Vector3 offset = { 0,2.5f,0.0f };
		float complement = 0.5f; // カメラ移動の際の補完

		float limitMinY;	// カメラ回転の限界(最小)
		float limitMaxY;	// カメラ回転の限界(最大)

		float followHeight;	// playerより少し上に配置するための距離

		float smoothTime = 0.1f;	// 追従の速度
		float maxSpeed = 10 ^ 8;	// 追従の最大速度

		int easingIndex;

		CameraParameter() {
			SetGroupName("Camera");
			SetName("FollowCamera");
		}

		json ToJson(const std::string& id) const override {
			return JsonBuilder(id)
				.Add("distance", distance)
				.Add("rotateDelta", rotateDelta)
				.Add("offset", offset)
				.Add("complement", complement)
				.Add("limitMinY", limitMinY)
				.Add("limitMaxY", limitMaxY)
				.Add("smoothTime", smoothTime)
				.Add("maxSpeed", maxSpeed)
				.Add("easingIndex", easingIndex)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			fromJson(jsonData, "distance", distance);
			fromJson(jsonData, "rotateDelta", rotateDelta);
			fromJson(jsonData, "offset", offset);
			fromJson(jsonData, "complement", complement);
			fromJson(jsonData, "limitMinY", limitMinY);
			fromJson(jsonData, "limitMaxY", limitMaxY);
			fromJson(jsonData, "smoothTime", smoothTime);
			fromJson(jsonData, "maxSpeed", maxSpeed);
			fromJson(jsonData, "easingIndex", easingIndex);
		}

		void Debug_Gui() override;
	};

	struct AnimationParameter : public IJsonConverter {
		Vector3 firstOffset;
		Vector3 targetOffset;
		float moveTime;
		int easingIndex;
		Color scaleColor;
		float vignettePower;

		AnimationParameter() { 
			SetGroupName("Camera");
			SetName("AnimationParameter");
		}

		json ToJson(const std::string& id) const override {
			return JsonBuilder(id)
				.Add("firstOffset", firstOffset)
				.Add("moveTime", moveTime)
				.Add("easingIndex", easingIndex)
				.Add("scaleColor", scaleColor)
				.Add("vignettePower", vignettePower)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			fromJson(jsonData, "firstOffset", firstOffset);
			fromJson(jsonData, "moveTime", moveTime);
			fromJson(jsonData, "easingIndex", easingIndex);
			fromJson(jsonData, "scaleColor", scaleColor);
			fromJson(jsonData, "vignettePower", vignettePower);
		}

		void Debug_Gui() override;
	};

public:

	FollowCamera() = default;
	~FollowCamera() override = default;

	void Finalize() override;
	void Init() override;
	void Update() override;

	void Debug_Gui() override;

private:	// private method

	void InputStick();

	void RotateCamera();

	void MoveCamera(const Vector3& target);

	void Shake();

	void FirstCameraMove();

public:		// accessor method

	void SetShake(float time, float strength);

	void SetTarget(Player* _target);

	void SetReticle(Reticle* reticle) { pReticle_ = reticle; }

	Quaternion GetAngleX();

	void SetAngleZ(float _angle) { angle_.z = _angle; }

private:

	// 他クラス ------------------------------------------------

	Player* pTarget_ = nullptr;

	Reticle* pReticle_ = nullptr;

	// Parameter ------------------------------------------------

	QuaternionSRT pivotSRT_;

	CameraParameter followCamera_;
	CameraParameter initFollowCamera_;

	Vector3 angle_ = {};

	const float kDeadZone_ = 0.4f;
	Vector2 stick_;

	Vector3 velocity_;

	float rotateLength_;

	Vector3 prePosition_;

	// Shake ------------------------------------------------
	float shakeTimer_ = 1.0f;
	float shakeTime_ = 1.0f;
	float shakeStrength_;

	// Animation ------------------------------------------------
	AnimationParameter animationParam_;
	float animationTimer_;
	bool isAnimationFinish_;
	std::shared_ptr<Grayscale> grayscale_;
	std::shared_ptr<Vignette> vignette_;

};

