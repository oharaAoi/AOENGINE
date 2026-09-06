#include "ParticleSceneObject.h"
#include "Engine/Module/Components/Effect/BaseParticles.h"
#include "Engine/System/Manager/ParticleManager.h"
#include <filesystem>

using namespace AOENGINE;

ParticleSceneObject::~ParticleSceneObject() {
	Stop();
	if (effect_.IsAlive()) { effect_.Destroy(); }
	if (cpu_) { ParticleManager::GetInstance()->DeleteParticles(cpu_); cpu_ = nullptr; }
}

void ParticleSceneObject::Init() {
	isDestroy_ = false;
	transform_.Init();
	if (autoPlay_) { Play(); }
}

void ParticleSceneObject::SetAsset(ParticleSceneAssetType type, const std::string& asset) {
	Stop();
	if (effect_.IsAlive()) { effect_.Destroy(); }
	if (cpu_) { ParticleManager::GetInstance()->DeleteParticles(cpu_); cpu_ = nullptr; }
	assetType_ = type; asset_ = asset; played_ = false;
}

void ParticleSceneObject::SetAssetFromPath(const std::string& path) {
	const std::filesystem::path filePath(path);
	if (filePath.extension() != ".json") { return; }
	// CompositeはParticleEditorがEffect/Compositeへ保存する。CPU粒子はEffect/CPU配下。
	const bool isEffect = filePath.parent_path().filename() == "Composite";
	SetAsset(isEffect ? ParticleSceneAssetType::Effect : ParticleSceneAssetType::Cpu,
		filePath.stem().string());
}

void ParticleSceneObject::ApplyParent() {
	if (cpu_) { cpu_->SetParentMatrix(transform_.GetWorldMatrix()); }
	if (effect_.IsAlive()) { effect_.SetParent(parentTransform_); }
}

void ParticleSceneObject::SetParentTransform(WorldTransform* parent) {
	parentTransform_ = parent;
	if (parent) { transform_.SetParent(parent->GetWorldMatrix()); }
	else { transform_.ClearParent(); }
	if (cpu_) { cpu_->SetParentMatrix(transform_.GetWorldMatrix()); }
	if (effect_.IsAlive()) { effect_.SetParent(parent); }
}

void ParticleSceneObject::EnsureRuntime() {
	if (asset_.empty()) { return; }
	if (assetType_ == ParticleSceneAssetType::Cpu && !cpu_) {
		cpu_ = ParticleManager::GetInstance()->CreateParticle(asset_);
		if (cpu_) { cpu_->SetParentMatrix(transform_.GetWorldMatrix()); }
	} else if (assetType_ == ParticleSceneAssetType::Effect && !effect_.IsAlive()) {
		effect_ = ParticleEffectManager::GetInstance()->Play(asset_, transform_.GetWorldPos());
		if (effect_.IsAlive()) { effect_.SetParent(parentTransform_); }
	}
}

void ParticleSceneObject::Play() {
	EnsureRuntime();
	if (cpu_) { cpu_->Reset(); cpu_->SetIsStop(false); }
	if (effect_.IsAlive()) { effect_.Restart(); }
	played_ = true;
}

void ParticleSceneObject::Stop() {
	if (cpu_) { cpu_->SetIsStop(true); }
	if (effect_.IsAlive()) { effect_.Stop(); }
	played_ = false;
}

void ParticleSceneObject::Update() {
	transform_.Update();
	if (autoPlay_ && !played_) { Play(); }
	ApplyParent();
	if (effect_.IsAlive()) { effect_.SetPosition(transform_.GetTranslate()); }
}
