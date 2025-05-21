#include "ParticleManager.h"
#include "Engine/Core/GraphicsContext.h"

ParticleManager::~ParticleManager() {
	Finalize();
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

void ParticleManager::Update() {
	for (auto& particle : particlesList_) {
		particleRenderer_->Update(particle->GetName(), particle->GetData());
	}
}

void ParticleManager::PostUpdate() {
	particleRenderer_->PostUpdate();
}

void ParticleManager::Draw() const {
	particleRenderer_->Draw(GraphicsContext::GetInstance()->GetCommandList());
}

void ParticleManager::AddParticle(const std::string& id, Mesh* _pMesh, Material* _pMaterial, bool isAddBlend) {
	particleRenderer_->AddParticle(id, _pMesh, _pMaterial, isAddBlend);
}

void ParticleManager::AddParticleList(BaseParticles* particles) {
	particlesList_.emplace_back(particles);
	particleRenderer_->AddParticle(particles->GetName(),
								   particles->GetGeometryObject()->GetMesh(),
								   particles->GetGeometryObject()->GetMaterial(),
								   particles->GetIsAddBlend());
}
