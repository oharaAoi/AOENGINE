#pragma once
#include "Engine/Module/PostEffect/GlitchNoise.h"
#include <Game/State/ICharacterState.h>

class Player;

class PlayerKnockbackState :
	public ICharacterState<Player> {
public:

	PlayerKnockbackState() = default;
	~PlayerKnockbackState() = default;

	void OnStart() override;
	void OnUpdate() override;
	void OnExit() override;

private:

	void Knockback();

private:

	float timer_ = 0.0f;
	float knockbackTime_ = 0.0f;

	float strength_;

	Vector3 velocity_;
	Vector3 acceleration_;

	std::shared_ptr<GlitchNoise> glitchNoise_;

};

