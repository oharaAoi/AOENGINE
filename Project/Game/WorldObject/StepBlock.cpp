#include "StepBlock.h"

#include "Engine/System/Manager/ParticleManager.h"
#include "Engine/Module/Components/WorldTransform.h"
#include "Engine/Module/Components/GameObject/BaseGameObject.h"

using namespace AOENGINE;

void StepBlock::OnBrokenEffect() const{
	auto particle = ParticleManager::GetInstance()->CreateParticle("BlockBroken");
	particle->SetPos(this->GetGameObject()->GetTransform()->GetTranslate());
}
