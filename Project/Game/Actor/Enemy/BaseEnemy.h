#pragma once
// Engine
#include "Engine/Module/Components/GameObject/BaseEntity.h"

enum class EnemyType {
	Short,		// 近距離
	Mid,		// 中距離
	Long		// 遠距離
};

/// <summary>
/// Enemyの基底となるクラス
/// </summary>
class BaseEnemy :
	public AOENGINE::BaseEntity {
public: // コンストラクタ

	BaseEnemy() = default;
	~BaseEnemy() = default;

public:

	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Init() = 0;

	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update() = 0;
	
	/// <summary>
	/// 編集処理
	/// </summary>
	virtual void Debug_Gui() override;

private:



};

