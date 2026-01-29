#pragma once
#include "Game/Actor/Enemy/BaseEnemy.h"

/// <summary>
/// 近距離型の敵クラス
/// </summary>
class ShortRangeEnemy :
	public BaseEnemy {
public: // コンストラクタ

	ShortRangeEnemy() = default;
	~ShortRangeEnemy() = default;

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

