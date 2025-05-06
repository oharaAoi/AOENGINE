#pragma once
#include "Engine/Components/Attribute/AttributeGui.h"
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

		json ToJson(const std::string& id) const override {
			return JsonBuilder(id)
				.Add("distance", distance)
				.Add("rotateDelta", rotateDelta)
				.Add("offset", offset)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			fromJson(jsonData, "distance", distance);
			fromJson(jsonData, "rotateDelta", rotateDelta);
			fromJson(jsonData, "offset", offset);
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

public:		// accessor method

	void SetTarget(Player* _target) { pTarget_ = _target; }

	void SetReticle(Reticle* reticle) { pReticle_ = reticle; }

	Quaternion GetAngleX();

private:

	// 他クラス ------------------------------------------------

	Player* pTarget_ = nullptr;

	Reticle* pReticle_ = nullptr;

	// Parameter ------------------------------------------------

	CameraParameter followCamera_;

	Vector2 angle_ = {};
	std::pair<float, float> angleLimitY_ = { -kPI / 16.0f, kPI / 3.0f };

	const float kDeadZone_ = 0.1f;
	Vector2 stick_;
};

