#include "ParticleManager.h"
#include "Engine.h"

ParticleManager::~ParticleManager() {
}

ParticleManager* ParticleManager::GetInstance() {
	static ParticleManager instance;
	return &instance;
}

void ParticleManager::Finalize() {
	particleRenderer_.reset();
}

void ParticleManager::Init() {
	particleRenderer_ = std::make_unique<ParticleInstancingRenderer>();
	particleRenderer_->Init(1000);

}

void ParticleManager::Update(const std::string& id, const std::vector<ParticleInstancingRenderer::ParticleData>& particleData) {
	particleRenderer_->Update(id, particleData);
}

void ParticleManager::PostUpdate() {
	particleRenderer_->PostUpdate();
}

void ParticleManager::Draw() const {
	particleRenderer_->Draw(Engine::GetCommandList());
}

void ParticleManager::AddParticle(const std::string& id, Mesh* _pMesh, Material* _pMaterial) {
	particleRenderer_->AddParticle(id, _pMesh, _pMaterial);
}
