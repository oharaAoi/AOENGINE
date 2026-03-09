#pragma once
// c++
#include <memory>
// engine
#include "Engine/Module/Components/Attribute/AttributeGui.h"
// game
#include "Game/Actor/Boss/Boss.h"
#include "Game/Actor/Boss/Bullet/BossBulletManager.h"
#include "Game/Actor/Player/Player.h"
#include "Game/Actor/Weapon/Flamethrower.h"
#include "Game/UI/Boss/BossUIs.h"
#include "Game/Camera/FollowCamera.h"

/// <summary>
/// Boss関連の親
/// </summary>
class BossRoot :
	public AOENGINE::AttributeGui {
public:

	BossRoot() = default;
	~BossRoot() = default;

public:

	// 初期化
	void Init();
	// 更新
	void Update();

	// 編集
	void Debug_Gui() override;

public:

	void SetBoss(Boss* boss);
	Boss* GetBoss() { return boss_; }

	BossBulletManager* GetBulletManager() { return bulletManager_.get(); }

	void SetPlayerPosition(const Math::Vector3& _position) { playerPosition_ = _position; }

	void SetUIs(BossUIs* _pBossUIs) { boss_->SetUIs(_pBossUIs); }

	void SetPlayer(Player* _player) { pPlayer_ = _player; }

	void SetTargetTransform(AOENGINE::WorldTransform* _transform) { boss_->SetTargetTransform(_transform); }

	void SetFollowCamera(FollowCamera* pFollowCamera) { 
		pFollowCamera_ = pFollowCamera;
		boss_->SetFollowCamera(pFollowCamera);
	}

private:

	Boss* boss_;
	std::unique_ptr<BossBulletManager> bulletManager_;

	// 他クラス情報
	Math::Vector3 playerPosition_;
	Player* pPlayer_ = nullptr;
	FollowCamera* pFollowCamera_ = nullptr;// 武器

};

