#include "BossDefeateExlosionAnimation.h"
#include "Engine/System/Manager/PrefabManager.h"
#include "Engine/System/Manager/ParticleManager.h"
#include "Engine/Module/Components/GameObject/BaseGameObject.h"
#include "Engine/Lib/GameTimer.h"
#include "Engine/Lib/Math/MyRandom.h"

#include "Game/Actor/Boss/BossDefeateAnimation/BossDefeateAnimation.h"
#include "Game/Actor/Boss/Boss.h"

void BossDefeateExlosionAnimation::Init() {
	explosionParameter_.Load();

	timer_ = AOENGINE::Timer(explosionParameter_.exlosionCoolTime);
	finalExplosionTimer_ = AOENGINE::Timer(explosionParameter_.finalExlosionTime);
	count_ = 0;
}

void BossDefeateExlosionAnimation::Update(Boss& boss) {
	if (count_ >= explosionParameter_.explosionCount) { 
		if (!finalExplosionTimer_.Run(AOENGINE::GameTimer::DeltaTime())) {
			AOENGINE::SceneObject* obj =
				AOENGINE::PrefabManager::GetInstance()->Instantiate("Exploasion");
			AOENGINE::BaseGameObject* gameObject =
				dynamic_cast<AOENGINE::BaseGameObject*>(obj);
			gameObject->GetTransform()->SetTranslate(boss.GetPosition());
			gameObject->GetTransform()->SetScale(CVector3::UNIT * 2.f);
			Engine::GetSoundManager()->Play("GameClearEffect");
			animation_->ChangeNext();
		}
		return; 
	}

	if (timer_.Run(AOENGINE::GameTimer::DeltaTime())) { return; }

	AOENGINE::SceneObject* obj =
		AOENGINE::PrefabManager::GetInstance()->Instantiate("Exploasion");
	AOENGINE::BaseGameObject* gameObject =
		dynamic_cast<AOENGINE::BaseGameObject*>(obj);
	if (gameObject) {
		Math::Vector3 createPos = boss.GetPosition();
		createPos += Math::Vector3(
			Random::RandomFloat(-explosionParameter_.explosionRange, explosionParameter_.explosionRange),
			Random::RandomFloat(-explosionParameter_.explosionRange, explosionParameter_.explosionRange), 
			Random::RandomFloat(-explosionParameter_.explosionRange, explosionParameter_.explosionRange));
		gameObject->GetTransform()->SetTranslate(createPos);
	}

	count_++;
	Engine::GetSoundManager()->Play("BossDeadExploade");

	if (count_ >= explosionParameter_.explosionCount) {
		flashParticle_ = AOENGINE::ParticleManager::GetInstance()->CreateParticle("BossDefeateFlash");
		flashParticle_->SetParent(boss.GetTransform());
		flashParticle_->SetIsStop(false);
	}

	timer_.Reset();
}
