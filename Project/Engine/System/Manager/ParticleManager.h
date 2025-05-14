#pragma once
#include <string>
#include <memory>
#include "Engine/Render/ParticleInstancingRenderer.h"
#include "Engine/Module/Components/Effect/BaseParticles.h"

class ParticleManager {
public:

	ParticleManager() = default;
	~ParticleManager();
	ParticleManager(const ParticleManager&) = delete;
	const ParticleManager& operator=(const ParticleManager&) = delete;

	static ParticleManager* GetInstance();

	void Finalize();

	void Init();

	void Update();

	void PostUpdate();

	void Draw() const;

public:

	/// <summary>
	/// Particleを実際に追加する
	/// </summary>
	/// <param name="id">: fileName</param>
	/// <param name="_pMesh">: mesh</param>
	/// <param name="_pMaterial">: material</param>
	/// <param name="isAddBlend">: どのようなBlendModeで行うか</param>
	void AddParticle(const std::string& id, Mesh* _pMesh, Material* _pMaterial, bool isAddBlend);

	/// <summary>
	/// View系のMatrixを設定する
	/// </summary>
	/// <param name="view">: viewMatrix</param>
	/// <param name="bill">: billBordMatrix</param>
	void SetView(const Matrix4x4& view, const Matrix4x4& bill) {
		particleRenderer_->SetView(view, bill);
	}

	void AddParticleList(BaseParticles* particles);

private:

	std::unique_ptr<ParticleInstancingRenderer> particleRenderer_;

	std::list<BaseParticles*> particlesList_;

};