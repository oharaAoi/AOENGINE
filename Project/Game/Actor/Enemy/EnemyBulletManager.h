#pragma once
// engine
#include "Engine/Lib/Math/Vector3.h"
// Game
#include "Game/Manager/BaseBulletManager.h"

/// <summary>
/// Enemyのbulletを管理するためのクラス
/// </summary>
class EnemyBulletManager final :
	public BaseBulletManager {
public: // コンストラクタ

	EnemyBulletManager() = default;
	~EnemyBulletManager() override;

public:

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

