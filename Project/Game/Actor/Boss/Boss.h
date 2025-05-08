#pragma once
#include <memory>
// Engine
#include "Engine/Components/GameObject/BaseGameObject.h"
// Game
#include "Game/State/StateMachine.h"

class Boss :
	public BaseGameObject {
public:

	Boss() = default;
	~Boss() = default;

	void Finalize() override;
	void Init() override;
	void Update() override;
	void Draw() const override;

#ifdef _DEBUG
	void Debug_Gui() override;
#endif // _DEBUG

public:

	StateMachine<Boss>* GetState() { return stateMachine_.get(); }

private:

	// state --------------------------------------------------
	std::unique_ptr<StateMachine<Boss>> stateMachine_;

};

