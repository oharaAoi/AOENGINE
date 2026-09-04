#include "PlayerToBossAttackCallBacks.h"

#include "Game/Actor/Boss/Boss.h"

void PlayerToBossAttackCallBacks::Init() {
	SetCallBacks();

	// 火球とビーム、どちらも同じコールバックで受ける
	SetPair(pCollisionManager_, "Player", kFireballName_);
	SetPair(pCollisionManager_, "Player", kBeamName_);
}

void PlayerToBossAttackCallBacks::CollisionEnter(AOENGINE::BaseCollider* const, AOENGINE::BaseCollider* const attack) {
	// 触れた瞬間にダメージを与える
	ApplyDamage(attack);
}

void PlayerToBossAttackCallBacks::CollisionStay(AOENGINE::BaseCollider* const, AOENGINE::BaseCollider* const attack) {
	// ビームは判定が数秒続くので、触れ続けている間も見る。
	// 多重ヒットは Player 側の無敵時間で防いでいる
	ApplyDamage(attack);
}

void PlayerToBossAttackCallBacks::CollisionExit(AOENGINE::BaseCollider* const, AOENGINE::BaseCollider* const) {
	// 離れた時は何もしない
}

void PlayerToBossAttackCallBacks::ApplyDamage(AOENGINE::BaseCollider* const attack) {
	if (!pPlayer_ || !pBoss_ || !attack) {
		return;
	}

	const BossParameter& param = pBoss_->GetParameter();

	// 当たった攻撃の種類でダメージ量を選ぶ
	float damage = 0.0f;
	if (attack->GetCategoryName() == kFireballName_) {
		damage = param.fireballDamage;
	} else if (attack->GetCategoryName() == kBeamName_) {
		damage = param.beamDamage;
	} else {
		return;
	}

	pPlayer_->TakeDamage(damage);
}
