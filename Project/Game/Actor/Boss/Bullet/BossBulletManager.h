#pragma once
// engine
#include "Engine/Lib/Math/Vector3.h"
// game
#include "Game/Manager/BaseBulletManager.h"

/// <summary>
/// BossのBulletを管理するクラス
/// </summary>
class BossBulletManager final :
	public BaseBulletManager {
public:

	BossBulletManager() = default;
	~BossBulletManager() override;

	/// <summary>
	/// 初期化
	/// </summary>
	void Init() override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update()override;

	
public: // member method

	void SetPlayerPosition(const Math::Vector3& pos) { playerPosition_ = pos; }

private:

	Math::Vector3 playerPosition_;

};

