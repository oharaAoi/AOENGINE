#pragma once
#include "Game/Actor/Base/BaseAction.h"

class Boss;

class BossActionShotMissile :
	public BaseAction<Boss> {
public:

	BossActionShotMissile() = default;
	~BossActionShotMissile() override = default;

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

	void Shot();

private :
	
	// アクション時間
	float actionTimer_;

	// 弾を打ち終わったかのフラグ
	bool isFinishShot_;
	float shotInterval_;

	float bulletSpeed_;

	const uint32_t kFireCount_ = 4;
	uint32_t fireCount_;

};