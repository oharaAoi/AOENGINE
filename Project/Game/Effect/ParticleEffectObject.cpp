#include "ParticleEffectObject.h"

/// engine
#include "Engine/Module/Components/Effect/BaseParticles.h"
#include "Engine/System/Manager/ParticleManager.h"

ParticleEffectObject::ParticleEffectObject(const std::string& particleName){
	pParticle_ = AOENGINE::ParticleManager::GetInstance()->CreateParticle(particleName);
}

ParticleEffectObject::~ParticleEffectObject(){
	// 親(EffectObjectGroup、ひいては Launcher)が消える時にここが呼ばれる。
	// emitter を確実に破棄することで ParticleManager 側にリークを残さない
	if(pParticle_ != nullptr){
		AOENGINE::ParticleManager::GetInstance()->DeleteParticles(pParticle_);
		pParticle_ = nullptr;
	}
}

void ParticleEffectObject::SetParent(AOENGINE::WorldTransform* parent){
	if(pParticle_ == nullptr){
		return;
	}
	pParticle_->SetParent(parent);
}

void ParticleEffectObject::SetLocalPosition(const Math::Vector3& position){
	if(pParticle_ == nullptr){
		return;
	}
	pParticle_->SetPos(position);
}

void ParticleEffectObject::Play(){
	if(pParticle_ == nullptr){
		return;
	}

	// 複合エフェクトのノードは delay を持つため、分解後もそれぞれの
	// 演出オブジェクト単体で再現できるように、delay が無ければ即座に射出し、
	// あれば止めたまま待ち状態に入って Update() で経過を見る
	if(delay_ <= 0.0f){
		pParticle_->Reset();
	} else{
		pParticle_->SetIsStop(true);
		waitTimer_ = delay_;
		isWaiting_ = true;
	}
}

void ParticleEffectObject::Stop(){
	if(pParticle_ == nullptr){
		return;
	}
	pParticle_->SetIsStop(true);
	// 待っている最中に止められたら、待ち時間が経過してもそのまま開始しない
	isWaiting_ = false;
}

void ParticleEffectObject::Update(float deltaTime){
	if(!isWaiting_){
		return;
	}

	waitTimer_ -= deltaTime;
	if(waitTimer_ <= 0.0f){
		isWaiting_ = false;
		if(pParticle_ != nullptr){
			pParticle_->Reset();
		}
	}
}

bool ParticleEffectObject::IsAlive() const{
	return pParticle_ != nullptr;
}
