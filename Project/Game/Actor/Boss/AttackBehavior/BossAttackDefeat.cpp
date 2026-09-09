#include "BossAttackDefeat.h"

#include "Game/Actor/Boss/Boss.h"

///////////////////////////////////////////////////////////////////////////////////////////////
//  開始
///////////////////////////////////////////////////////////////////////////////////////////////

void BossAttackDefeat::Enter(Boss& boss) {
	(void)boss;

	elapsedTime_ = 0.0f;
	afterDefeatTime_ = 0.0f;
	isHidden_ = false;

	bossDefateAnimation_ = std::make_unique<BossDefeateAnimation>();
	bossDefateAnimation_->Init();
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  更新
///////////////////////////////////////////////////////////////////////////////////////////////

void BossAttackDefeat::Update(Boss& boss, float deltaTime) {

	if (!isHidden(boss,deltaTime)) {
		return;
	}

	// 非表示
	isHidden_ = true;
	boss.SetRendering(false);

	// シーンに切り替わるまでのタイムを更新
	afterDefeatTime_+= deltaTime;

	if (afterDefeatTime_ < boss.GetParameter().sceneChangeTimeAfterHiden_) {
		return;
	}

	// ここまで見せたらシーンを進めてよい
	boss.SetDefeatFinished(true);
}


bool BossAttackDefeat::isHidden(Boss& boss, float deltaTime) {

	if (isHidden_) {
		return true;
	}

	// 倒れたところを見せてから姿を消す
	elapsedTime_ += deltaTime;
	if (elapsedTime_ < boss.GetParameter().defeatHideTime) {
		bossDefateAnimation_->Update(boss);

		if (bossDefateAnimation_->IsFinish()) {
			return true;
		}

		return false;
	}

	return true;
}