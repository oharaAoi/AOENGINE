#pragma once
#include "Game/Actor/Base/BaseAction.h"

// 前方宣言
class Player;

class PlayerActionMove :
	public BaseAction<Player>{
public:

	PlayerActionMove() = default;
	~PlayerActionMove() = default;

	void Build() override;
	void OnStart() override;
	void OnUpdate() override;
	void OnEnd() override;

	void CheckNextAction() override;
	bool IsInput() override;

private:	// action

	void Move();

private:

	const float kDeadZone_ = 0.1f;
	Vector2 stick_;

	float speed_ = 4.0f;

};

