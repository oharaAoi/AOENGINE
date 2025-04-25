#pragma once
#include <string>
#include <memory>
#include "Engine/Render/ParticleInstancingRenderer.h"

class ParticleManager {
public:

	ParticleManager() = default;
	~ParticleManager();
	ParticleManager(const ParticleManager&) = delete;
	const ParticleManager& operator=(const ParticleManager&) = delete;

	static ParticleManager* GetInstance();

	void Finalize();

	void Init();

	void Update(const std::string& id, const std::vector<ParticleInstancingRenderer::ParticleData>& particleData);

	void PostUpdate();

	void Draw() const;

public:

	void AddParticle(const std::string& id, Mesh* _pMesh, Material* _pMaterial, bool isAddBlend);

	void SetView(const Matrix4x4& view, const Matrix4x4& bill) {
		particleRenderer_->SetView(view, bill);
	}

private:

	std::unique_ptr<ParticleInstancingRenderer> particleRenderer_;

};