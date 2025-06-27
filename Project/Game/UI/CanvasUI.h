#pragma once
#include <memory>
#include "Engine/Module/Components/Attribute/AttributeGui.h"
#include "Engine/Module/Components/2d/Sprite.h"
#include "Game/UI/Reticle.h"
#include "Game/UI/PostureStability.h"
#include "Game/UI/Player/EnergyOutput.h"

#include "Game/Actor/Player/Player.h"
#include "Game/Actor/Boss/Boss.h"
#include "Game/Camera/FollowCamera.h"
#include "Game/UI/Boss/BossUIs.h"
#include "Game/UI/Player/PlayerUIs.h"

/// <summary>
/// UIをまとめたクラス
/// </summary>
class CanvasUI :
	public AttributeGui {
public:	// base

	CanvasUI() = default;
	~CanvasUI() = default;

	void Init();

	void Update();

	void Draw() const;

	void Debug_Gui();

public:		// accessor method

	void SetPlayer(Player* _player) { pPlayer_ = _player; }

	void SetBoss(Boss* _boss) { pBoss_ = _boss; }

	void SetFollowCamera(FollowCamera* _followCamera) { pFollowCamera_ = _followCamera; }

	Reticle* GetReticle() { return reticle_.get(); }

	PostureStability* GetPostureStability() { return postureStability_.get(); }

	BossUIs* GetBossUIs() const { return bossUIs_.get(); }

private:

	// ---------------------------------------------------
	// 他クラス
	Player* pPlayer_;
	Boss* pBoss_ = nullptr;
	FollowCamera* pFollowCamera_;

	// ---------------------------------------------------
	// player
	std::unique_ptr<PlayerUIs> playerUIs_;
	std::unique_ptr<Reticle> reticle_;	// reticle
	std::unique_ptr<EnergyOutput> energyOutput_;	// energy
	std::unique_ptr<PostureStability> postureStability_; // 姿勢安定度ゲージ
	std::unique_ptr<Sprite> boostOn_; // 姿勢安定度ゲージ
	Vector2 boostOnPos_ = {940.0f, 640.0f};
	Vector2 boostOnScale_ = {0.3f, 0.3f};

	// boss
	std::unique_ptr<BossUIs> bossUIs_;

};

