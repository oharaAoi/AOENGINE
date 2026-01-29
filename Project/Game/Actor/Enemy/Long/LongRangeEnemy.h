#pragma once
#include "Game/Actor/Enemy/BaseEnemy.h"

/// <summary>
/// 遠距離型の敵
/// </summary>
class LongRangeEnemy :
	public BaseEnemy {
public: // コンストラクタ

	LongRangeEnemy() = default;
	~LongRangeEnemy() = default;

public: // method

	/// <summary>
	/// 初期化
	/// </summary>
	void Init() override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;

	/// <summary>
	/// 編集
	/// </summary>
	void Debug_Gui() override;

};

