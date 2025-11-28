#pragma once
#include <Game/State/ICharacterState.h>

class Player;

class PlayerIdleState :
	public ICharacterState<Player>{
public:

	PlayerIdleState() = default;
	~PlayerIdleState() = default;

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

};

