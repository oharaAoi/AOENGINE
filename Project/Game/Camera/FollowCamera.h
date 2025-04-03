#pragma once
#include "Engine/Components/Attribute/AttributeGui.h"
#include "Game/Camera/BaseCamera.h"
#include <utility>

class Player;

/// <summary>
/// Playerに追従するCamera
/// </summary>
class FollowCamera :
	public BaseCamera,
	public AttributeGui {
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

	Quaternion GetAngleX();

private:

	Player* pTarget_ = nullptr;

	Vector3 offset_ = {0,2.5f,0.0f};
	Vector2 angle_ = {};
	std::pair<float, float> angleLimitY_ = { -kPI / 16.0f, kPI / 3.0f };

	const float kDeadZone_ = 0.1f;
	Vector2 stick_;

	float rotateDelta_ = 0.04f;

};

