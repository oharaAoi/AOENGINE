#pragma once
#include "Engine/Components/GameObject/BaseGameObject.h"
#include "Game/State/StateMachine.h"

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

	std::unique_ptr<StateMachine<Player>> stateMachine_;

};

