#pragma once
#include <functional>
#include "Engine/Lib/Math/Quaternion.h"
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

	/// <summary>
	/// 発射
	/// </summary>
	void Shot();

	/// <summary>
	/// Playerの方向を向かせる処理
	/// </summary>
	void LookPlayer();

private :

	Quaternion playerToRotation_;

	float bulletSpeed_;

	// LookPlayerに関する変数
	float lookTime_ = 1.f;

	// 弾を打ち終わったかのフラグ
	bool isFinishShot_;

	std::function<void()> mainAction_;

};

