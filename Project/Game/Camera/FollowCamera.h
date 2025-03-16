#pragma once
#include "Engine/Components/Attribute/AttributeGui.h"
#include "Game/Camera/BaseCamera.h"

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

public:		// accessor method

	void SetTarget(Player* _target) { pTarget_ = _target; }

private:

	Player* pTarget_ = nullptr;

	Vector3 offset_ = {0,2.5f,-15.0f};

};

