#pragma once
#include "Game/Actor/Enemy/BaseEnemy.h"

/// <summary>
/// 中距離の敵クラス
/// </summary>
class MidRangeEnemy :
	public BaseEnemy {
public: // コンストラクタ

	MidRangeEnemy() = default;
	~MidRangeEnemy() = default;

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

