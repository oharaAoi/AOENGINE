#pragma once
// Engin
#include "Engine/Components/GameObject/BaseGameObject.h"
// Game
#include "Game/State/StateMachine.h"
#include "Game/Manager/ActionManager.h"

class Player :
	public BaseGameObject {
public:

	Player();
	~Player();

	void Finalize() override;
	void Init() override;
	void Update() override;
	void Draw() const override;

#ifdef _DEBUG
	void Debug_Gui() override;
#endif // _DEBUG

public:		// accessor method

	StateMachine<Player>* GetState() { return stateMachine_.get(); }

private:

	// 他クラス ------------------------------------------------



	// state --------------------------------------------------
	std::unique_ptr<StateMachine<Player>> stateMachine_;

	ActionManager<Player> actionManager_;

};

