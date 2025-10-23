#pragma once
#include <Game/State/ICharacterState.h>
#include "Engine/Module/Components/Animation/VectorTween.h"

class Boss;

/// <summary>
/// 待機時のステート
/// </summary>
class BossIdleState :
	public ICharacterState<Boss> {
public:

	BossIdleState() = default;
	~BossIdleState() = default;

public:

	// 初期化
	void OnStart() override;
	// 更新
	void OnUpdate() override;
	// 終了時
	void OnExit() override;
	// 編集処理
	void Debug_Gui() override {};

private:

	VectorTween<float> floatingTween_;

};

