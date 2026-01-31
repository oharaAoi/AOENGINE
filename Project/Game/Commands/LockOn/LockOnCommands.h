#pragma once
#include "Game/Actor/Enemy/EnemyManager.h"
#include "Game/UI/Reticle.h"

/// <summary>
/// LockOnコマンドのインターフェイス
/// </summary>
class ILockOnCommand {
public:

	ILockOnCommand() = default;
	virtual ~ILockOnCommand() = default;
	virtual void Execute() = 0;
};

/// <summary>
/// ロックオン時のコマンド
/// </summary>
class LockOnCommand :
	public ILockOnCommand {
public: // コンストラクタ

	LockOnCommand(EnemyManager* enemyManager, Reticle* reticle);
	~LockOnCommand() = default;

public: // method

	/// <summary>
	/// 実行処理
	/// </summary>
	void Execute() override;

private:

	EnemyManager* pEnemyManager_ = nullptr;
	Reticle* pReticle_ = nullptr;

};