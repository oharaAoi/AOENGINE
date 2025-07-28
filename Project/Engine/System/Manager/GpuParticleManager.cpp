#include "GpuParticleManager.h"
#include "Render.h"
#include "Engine/System/Editer/Window/EditorWindows.h"

GpuParticleManager::~GpuParticleManager() {
	emitterList_.clear();
	renderer_.reset();
}

GpuParticleManager* GpuParticleManager::GetInstance() {
	static GpuParticleManager instance;
	return &instance;
}

void GpuParticleManager::Finalize() {
	emitterList_.clear();
	renderer_.reset();
}

void GpuParticleManager::Debug_Gui() {
}

void GpuParticleManager::Init() {
	SetName("GpuParticleManager");

	renderer_ = std::make_unique<GpuParticleRenderer>();
	renderer_->Init(10240);

	EditorWindows::AddObjectWindow(this, "GpuParticleManager");
}

void GpuParticleManager::Update() {
	renderer_->SetView(Render::GetViewProjectionMat(), Render::GetCameraRotate().MakeMatrix());
	renderer_->Update();

	for (auto& emitter : emitterList_) {
		emitter->Update();
	}
}

void GpuParticleManager::Draw() const {
	renderer_->Draw();
}

GpuParticleEmitter* GpuParticleManager::CreateEmitter(const std::string& particlesFile) {
	auto& newParticles = emitterList_.emplace_back(std::make_unique<GpuParticleEmitter>());
	newParticles->Init(particlesFile);
	newParticles->SetParticleResourceHandle(renderer_->GetResourceHandle());
	newParticles->SetFreeListIndexHandle(renderer_->GetFreeListIndexHandle());
	newParticles->SetFreeListHandle(renderer_->GetFreeListHandle());
	newParticles->SetMaxParticleResource(renderer_->GetMaxBufferResource());
	AddChild(newParticles.get());
	return newParticles.get();
}