#pragma once
#include "Engine/Module/Components/Attribute/AttributeGui.h"
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
		float distance = 20.0f;
		float rotateDelta = 0.04f;
		Vector3 offset = { 0,2.5f,0.0f };
		float complement = 0.5f; // カメラ移動の際の補完

		int easingIndex;

		json ToJson(const std::string& id) const override {
			return JsonBuilder(id)
				.Add("distance", distance)
				.Add("rotateDelta", rotateDelta)
				.Add("offset", offset)
				.Add("complement", complement)
				.Add("easingIndex", easingIndex)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			fromJson(jsonData, "distance", distance);
			fromJson(jsonData, "rotateDelta", rotateDelta);
			fromJson(jsonData, "offset", offset);
			fromJson(jsonData, "complement", complement);
			fromJson(jsonData, "easingIndex", easingIndex);
		}
	};

public:

	FollowCamera() = default;
	~FollowCamera() override = default;

	void Finalize() override;
	void Init() override;
	void Update() override;

#ifdef _DEBUG
	void Debug_Gui() override;
#endif

private:	// private method

	void RotateCamera();

	void Shake();

public:		// accessor method

	void SetShake(float time, float strength);

	void SetTarget(Player* _target) { pTarget_ = _target; }

	void SetReticle(Reticle* reticle) { pReticle_ = reticle; }

	Quaternion GetAngleX();

private:

	// 他クラス ------------------------------------------------

	Player* pTarget_ = nullptr;

	Reticle* pReticle_ = nullptr;

	// Parameter ------------------------------------------------

	CameraParameter followCamera_;

	Vector3 prePosition_;

	Vector2 angle_ = {};
	std::pair<float, float> angleLimitY_ = { -kPI / 16.0f, kPI / 3.0f };

	const float kDeadZone_ = 0.1f;
	Vector2 stick_;

	// Shake ------------------------------------------------
	float shakeTimer_ = 1.0f;
	float shakeTime_ = 1.0f;
	float shakeStrength_;

};

