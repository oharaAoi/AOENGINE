#pragma once
#include "Game/Actor/Base/BaseAction.h"

class Boss;

/// <summary>
/// 全方位にミサイルを飛ばすアクション
/// </summary>
class BossActionAllRangeMissile :
	public BaseAction<Boss> {

public:

	BossActionAllRangeMissile() = default;
	~BossActionAllRangeMissile() override = default;

	void Build() override;
	void OnStart() override;
	void OnUpdate() override;
	void OnEnd() override;

	void CheckNextAction() override;
	bool IsInput() override;

#ifdef _DEBUG
	void Debug_Gui() override;
#endif // _DEBUG

private :

	void Shot();

};

