#pragma once
// engine
#include "Engine/Lib/Math/Vector3.h"
// Game
#include "Game/Manager/BaseBulletManager.h"

/// <summary>
/// PlayerのBulletを管理するクラス
/// </summary>
class PlayerBulletManager final :	
	public BaseBulletManager {
public:

	PlayerBulletManager() = default;
	~PlayerBulletManager() override;

public:

	/// <summary>
	/// 初期化
	/// </summary>
	void Init() override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update()override;

public:

	void SetPlayerTargetPos(const Math::Vector3& playerTargetPos);

private:

	Math::Vector3 playerTargetPos_;

};

