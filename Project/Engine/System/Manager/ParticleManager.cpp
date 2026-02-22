#include "ParticleManager.h"
#include "Engine/System/Editor/Window/EditorWindows.h"
#include "Engine/Core/GraphicsContext.h"
#include "Engine/Lib/GameTimer.h"
#include "Engine/Render.h"

using namespace AOENGINE;

ParticleManager::~ParticleManager() {
	Finalize();
}

AOENGINE::ParticleManager* AOENGINE::ParticleManager::GetInstance() {
	static ParticleManager instance;
	return &instance;
}

void ParticleManager::Finalize() {
	emitterList_.clear();
	particleRenderer_ = nullptr;
	ClearChild();
}

void ParticleManager::Debug_Gui() {

}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void ParticleManager::Init() {
	SetName("ParticleManager");

	particleRenderer_ = std::make_unique<ParticleInstancingRenderer>();
	particleRenderer_->Init(51600 * 2);

	AOENGINE::EditorWindows::AddObjectWindow(this, "ParticleManager");
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void ParticleManager::Update() {
	this->SetView(AOENGINE::Render::GetViewProjectionMat(), AOENGINE::Render::GetProjection2D(), Math::Matrix4x4::MakeUnit());

	// particleの更新
	particleUpdater_.Update();

	// emitterの更新
	for (auto& emitter : emitterList_) {
		if (emitter->GetChangeMesh()) {
			emitter->ChangeMesh();
			particleRenderer_->ChangeMesh(emitter->GetName(), emitter->GetMesh());
		}
		emitter->Update();
	}

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

void ParticleManager::Draw() const {
	particleRenderer_->Draw(AOENGINE::GraphicsContext::GetInstance()->GetCommandList());
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 設定時のみ行う処理
///////////////////////////////////////////////////////////////////////////////////////////////

AOENGINE::BaseParticles* ParticleManager::CreateParticle(const std::string& particlesFile) {
	auto& newParticles = emitterList_.emplace_back(std::make_unique<AOENGINE::BaseParticles>());
	newParticles->Init(particlesFile);
	std::string textureName = newParticles->GetUseTexture();
	newParticles->SetShareMaterial(
		particleRenderer_->AddParticle(newParticles->GetName(),
									   textureName,
									   newParticles->GetMesh(),
									   newParticles->GetIsAddBlend())
	);

	newParticles->GetShareMaterial()->SetAlbedoTexture(newParticles->GetUseTexture());
	particleUpdater_.Add(particlesFile);
	particleUpdater_.SetRuntimeAddBlend(particlesFile, newParticles->GetIsAddBlend());
	newParticles->SetParticlesList(particleUpdater_.GetParticles(particlesFile));
	AddChild(newParticles.get());
	return newParticles.get();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 設定時のみ行う処理
///////////////////////////////////////////////////////////////////////////////////////////////

void ParticleManager::DeleteParticles(AOENGINE::BaseParticles* ptr) {
	for (auto it = emitterList_.begin(); it != emitterList_.end(); ) {
		if (it->get() == ptr) {
			DeleteChild(it->get()); // 削除時の追加処理
			it = emitterList_.erase(it); // 要素の削除とイテレータ更新
		} else {
			++it;
		}
	}
}
