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
	/// View系のMatrixを設定する
	/// </summary>
	/// <param name="view">: viewMatrix</param>
	/// <param name="bill">: billBordMatrix</param>
	void SetView(const Matrix4x4& view, const Matrix4x4& bill) {
		particleRenderer_->SetView(view, bill);
	}

	//void AddParticleList(BaseParticles* particles);


	/// <summary>
	/// Particleを作成する
	/// </summary>
	/// <param name="particlesFile"></param>
	/// <returns></returns>
	BaseParticles* CrateParticle(const std::string& particlesFile);

	void DeleteParticles(BaseParticles* ptr);

private:

	std::unique_ptr<ParticleInstancingRenderer> particleRenderer_;

	std::list<std::unique_ptr<BaseParticles>> particlesList_;

};