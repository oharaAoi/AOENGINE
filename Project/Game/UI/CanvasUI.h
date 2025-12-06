#pragma once
#include <memory>
#include "Engine/Module/Components/Attribute/AttributeGui.h"
#include "Engine/Module/Components/2d/Sprite.h"
#include "Game/UI/Reticle.h"
#include "Game/UI/PostureStability.h"
#include "Game/UI/ClearNotificationUI.h"

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

public:

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="_isTutorial"></param>
	void Init(bool _isTutorial);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw() const;

	/// <summary>
	/// 編集
	/// </summary>
	void Debug_Gui();

public:		// accessor method

	void SetPlayer(Player* _player) { pPlayer_ = _player; }

	void SetBoss(Boss* _boss) { pBoss_ = _boss; }

	void SetFollowCamera(FollowCamera* _followCamera) { pFollowCamera_ = _followCamera; }

	Reticle* GetReticle() { return reticle_.get(); }

	BossUIs* GetBossUIs() const { return bossUIs_.get(); }

	bool IsFinishClearNotification() { return clearNotificationUI_->IsFinish(); }

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
	
	Math::Vector2 boostOnPos_ = {940.0f, 640.0f};
	Math::Vector2 boostOnScale_ = {0.3f, 0.3f};

	// boss
	std::unique_ptr<BossUIs> bossUIs_;

	// out game
	std::unique_ptr<ClearNotificationUI> clearNotificationUI_;

	std::unique_ptr<AOENGINE::Sprite> control_;

	bool isTutorial_;

};

