#pragma once
#include <memory>
#include "Engine/Module/Components/Attribute/AttributeGui.h"
#include "Engine/Module/Components/2d/Sprite.h"
// Game
#include "Game/UI/Reticle.h"
#include "Game/Actor/Player/Player.h"
#include "Game/Actor/Boss/Boss.h"
#include "Game/Camera/FollowCamera.h"
#include "Game/UI/Player/PlayerUIs.h"


/// <summary>
/// TutorialのUI
/// </summary>
class TutorialCanvas :
	public AOENGINE::AttributeGui {
public:

	TutorialCanvas() = default;
	~TutorialCanvas() = default;

public:

	// 初期化
	void Init();
	// 更新
	void Update();
	// 編集
	void Debug_Gui() override;

public:

	void SetPlayer(Player* _player) { pPlayer_ = _player; }

	void SetBoss(Boss* _boss) { pBoss_ = _boss; }

	void SetFollowCamera(FollowCamera* _followCamera) { pFollowCamera_ = _followCamera; }

	Reticle* GetReticle() { return reticle_.get(); }

private:

	// ---------------------------------------------------
	// 他クラス
	Player* pPlayer_;
	Boss* pBoss_ = nullptr;
	FollowCamera* pFollowCamera_;

	// player
	std::unique_ptr<PlayerUIs> playerUIs_;
	std::unique_ptr<Reticle> reticle_;	// reticle
	AOENGINE::Sprite* boostOn_; // 

	Math::Vector2 boostOnPos_ = { 940.0f, 640.0f };
	Math::Vector2 boostOnScale_ = { 0.3f, 0.3f };
};

