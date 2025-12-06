#pragma once
#include "Game/State/ICharacterState.h"
#include "Engine/Module/Components/Animation/VectorTween.h"

class Boss;

/// <summary>
/// 通常状態
/// </summary>
class BossStateNormal :
	public ICharacterState<Boss> {
public:

	BossStateNormal() = default;
	~BossStateNormal() = default;

public:

	// 初期化
	void OnStart() override;
	// 更新
	void OnUpdate() override;
	// 終了時
	void OnExit() override;
	// 編集処理
	void Debug_Gui() override;

public:

	AOENGINE::VectorTween<float> floatingTween_;

};

