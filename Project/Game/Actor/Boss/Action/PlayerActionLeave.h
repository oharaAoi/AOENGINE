#pragma once
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Game/Actor/Base/BaseAction.h"

class Boss;

/// <summary>
/// Bossの離れるアクション
/// </summary>
class PlayerActionLeave :
	public BaseAction<Boss> {
public:



public:

	PlayerActionLeave() = default;
	~PlayerActionLeave() = default;

	void Build() override;
	void OnStart() override;
	void OnUpdate() override;
	void OnEnd() override;

	void CheckNextAction() override;
	bool IsInput() override;

#ifdef _DEBUG
	void Debug_Gui() override;
#endif // _DEBUG

private:

	void Leave();

private:

};

