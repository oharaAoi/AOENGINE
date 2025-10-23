#pragma once
#include "Engine/Module/PostEffect/GlitchNoise.h"
#include <Game/State/ICharacterState.h>

class Player;

class PlayerKnockbackState :
	public ICharacterState<Player> {
public:

	PlayerKnockbackState() = default;
	~PlayerKnockbackState() = default;

public:

	// 初期化
	void OnStart() override;
	// 更新
	void OnUpdate() override;
	// 終了
	void OnExit() override;
	// 編集
	void Debug_Gui() override {};

private:

	// ノックバック処理
	void Knockback();

private:

	float timer_ = 0.0f;
	float knockbackTime_ = 0.0f;

	float strength_;

	Vector3 velocity_;
	Vector3 acceleration_;

	std::shared_ptr<GlitchNoise> glitchNoise_;

};

