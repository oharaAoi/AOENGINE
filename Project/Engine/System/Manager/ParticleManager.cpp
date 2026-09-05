#include "ParticleManager.h"
#include "Engine/System/Editor/Window/EditorWindows.h"
#include "Engine/Core/GraphicsContext.h"
#include "Engine/Lib/GameTimer.h"
#include "Engine/Render/Render.h"
#include "Engine/System/Manager/ParticleEffectManager.h"
#include <algorithm>

using namespace AOENGINE;

ParticleManager::~ParticleManager() {
	Finalize();
}

AOENGINE::ParticleManager* AOENGINE::ParticleManager::GetInstance() {
	static ParticleManager instance;
	return &instance;
}

void ParticleManager::Finalize() {
	externalEmitters_.clear();
	retiredEmitters_.splice(retiredEmitters_.end(), emitterList_);
	particleUpdater_.Finalize();
	if (particleRenderer_) { retiredRenderers_.push_back(std::move(particleRenderer_)); }
}

void ParticleManager::ReleaseRetiredResources() {
	retiredEmitters_.clear();
	retiredRenderers_.clear();
}

void ParticleManager::Debug_Gui() {

}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void ParticleManager::Init() {
	SetName("ParticleManager");

	particleRenderer_ = std::make_unique<ParticleInstancingRenderer>();
	particleRenderer_->Init(BaseParticles::kDefaultMaxParticles);

}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void ParticleManager::Update() {
	// 複合Effectの遅延再生とTransformを、各Emitterの更新より先に反映する。
	ParticleEffectManager::GetInstance()->Update();
	this->SetView(AOENGINE::Render::GetViewProjectionMat(), AOENGINE::Render::GetProjection2D(), Math::Matrix4x4::MakeUnit());

	// 新規生成の前に寿命切れParticleを回収する
	particleUpdater_.RemoveDeadParticles();

	// emitterの更新
	for (auto& emitter : emitterList_) {
		if (emitter->GetChangeMesh()) {
			emitter->ChangeMesh();
			particleRenderer_->ChangeMesh(emitter->GetRuntimeId(), emitter->GetMesh());
		}
		emitter->Update();
	}
	for (BaseParticles* emitter : externalEmitters_) {
		if (!emitter) { continue; }
		if (emitter->GetChangeMesh()) {
			emitter->ChangeMesh();
			particleRenderer_->ChangeMesh(emitter->GetRuntimeId(), emitter->GetMesh());
		}
		emitter->Update();
	}

	// 新しく生成したParticleも含めて更新・描画データを構築する
	particleUpdater_.Update();

	// renderの更新
	particleUpdater_.RendererUpdate(particleRenderer_.get());
	PostUpdate();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 全体更新後処理
///////////////////////////////////////////////////////////////////////////////////////////////

void ParticleManager::PostUpdate() {
	particleRenderer_->PostUpdate();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 設定時のみ行う処理
///////////////////////////////////////////////////////////////////////////////////////////////

void ParticleManager::Draw(const Math::Frustum& frustum) const {
	// 複数View描画ではUpdate時のCameraではなく、現在の描画Viewを使用する。
	particleRenderer_->SetView(
		AOENGINE::Render::GetViewProjectionMat(),
		AOENGINE::Render::GetProjection2D(),
		AOENGINE::Render::GetBillBordMat());
	particleRenderer_->Draw(AOENGINE::GraphicsContext::GetInstance()->GetCommandList(), &frustum);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 設定時のみ行う処理
///////////////////////////////////////////////////////////////////////////////////////////////

AOENGINE::BaseParticles* ParticleManager::CreateParticle(const std::string& particlesFile) {
	auto& newParticles = emitterList_.emplace_back(std::make_unique<AOENGINE::BaseParticles>());
	newParticles->Init(particlesFile, false);
	newParticles->SetRuntimeId(particlesFile + "#" + std::to_string(nextRuntimeId_++));
	std::string textureName = newParticles->GetUseTexture();
	newParticles->SetShareMaterial(
		particleRenderer_->AddParticle(newParticles->GetRuntimeId(),
									   textureName,
									   newParticles->GetMesh(),
									   newParticles->GetBlendMode(),
									   newParticles->GetMaxParticles())
	);

	newParticles->GetShareMaterial()->SetAlbedoTexture(newParticles->GetUseTexture());
	particleUpdater_.Add(newParticles->GetRuntimeId());
	particleUpdater_.SetRuntimeBlendMode(newParticles->GetRuntimeId(), newParticles->GetBlendMode());
	newParticles->SetParticlesList(particleUpdater_.GetParticles(newParticles->GetRuntimeId()));
	return newParticles.get();
}

bool ParticleManager::RegisterExternalParticle(BaseParticles* particle) {
	if (!particle || !particleRenderer_) { return false; }
	if (std::find(externalEmitters_.begin(), externalEmitters_.end(), particle) != externalEmitters_.end()) {
		return true;
	}

	particle->SetRuntimeId(particle->GetName() + "#" + std::to_string(nextRuntimeId_++));
	particle->SetShareMaterial(particleRenderer_->AddParticle(
		particle->GetRuntimeId(), particle->GetUseTexture(), particle->GetMesh(),
		particle->GetBlendMode(), particle->GetMaxParticles()));
	particleUpdater_.Add(particle->GetRuntimeId());
	particleUpdater_.SetRuntimeBlendMode(particle->GetRuntimeId(), particle->GetBlendMode());
	particle->SetParticlesList(particleUpdater_.GetParticles(particle->GetRuntimeId()));
	externalEmitters_.push_back(particle);
	return true;
}

void ParticleManager::UnregisterExternalParticle(BaseParticles* particle) {
	if (!particle) { return; }
	const auto it = std::find(externalEmitters_.begin(), externalEmitters_.end(), particle);
	if (it == externalEmitters_.end()) { return; }
	const std::string runtimeId = particle->GetRuntimeId();
	externalEmitters_.erase(it);
	particleUpdater_.Remove(runtimeId);
	if (particleRenderer_) { particleRenderer_->RemoveParticle(runtimeId); }
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 設定時のみ行う処理
///////////////////////////////////////////////////////////////////////////////////////////////

void ParticleManager::DeleteParticles(AOENGINE::BaseParticles* ptr) {
	for (auto it = emitterList_.begin(); it != emitterList_.end(); ) {
		if (it->get() == ptr) {
			particleUpdater_.Remove(ptr->GetRuntimeId());
			if (particleRenderer_) { particleRenderer_->RemoveParticle(ptr->GetRuntimeId()); }
			it = emitterList_.erase(it); // 要素の削除とイテレータ更新
		} else {
			++it;
		}
	}
}
