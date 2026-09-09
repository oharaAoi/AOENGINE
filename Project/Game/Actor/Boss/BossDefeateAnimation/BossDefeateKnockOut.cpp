#include "BossDefeateKnockOut.h"

#include "Engine/Module/Components/WorldTransform.h"
#include "Engine/System/Manager/ParticleManager.h"
#include "Engine/Lib/GameTimer.h"
#include "Engine/Core/Engine.h"

#include "Game/Actor/Boss/BossDefeateAnimation/BossDefeateAnimation.h"
#include "Game/Actor/Boss/Boss.h"

void BossDefeateKnockOut::Init() {
	knockOutParameter_.Load();

	timer_ = AOENGINE::Timer(knockOutParameter_.knockOutTime);
}

void BossDefeateKnockOut::Update(Boss& boss) {

	AOENGINE::WorldTransform* transform = boss.GetTransform();

	// 座標の更新
	Math::Vector3 pos = transform->GetWorldPos();
	pos += knockOutParameter_.knockOutDirection * knockOutParameter_.knockOutSpeed;
	transform->SetTranslate(pos);

	// 回転の更新
	Math::Quaternion rotate = transform->GetRotate();
	Math::Quaternion knockOutRotate = Math::Quaternion::AngleAxis(knockOutParameter_.knockOutRotateSpeed * kToRadian, CVector3::RIGHT);
	transform->SetRotate(knockOutRotate * rotate);

	const BossParameter& pram = boss.GetParameter();
	if (timer_.Run(AOENGINE::GameTimer::DeltaTime())) {
		Math::Vector3 scale = Math::Vector3::Lerp(pram.baseScale, CVector3::ZERO, timer_.t_);
		transform->SetScale(scale);
	} else {
		auto particle = AOENGINE::ParticleManager::GetInstance()->CreateParticle("CrossLightParticle");
		particle->SetPos(pos);
		animation_->ChangeNext();
		Engine::GetSoundManager()->Play("BossDeadStar");
	}
}
