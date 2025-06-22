#pragma once
#include <string>
#include <memory>
#include "Engine/Lib/ParticlesData.h"
#include "Engine/Render/ParticleInstancingRenderer.h"
#include "Engine/Module/Components/Effect/BaseParticles.h"
#include "Engine/Module/Components/Attribute/AttributeGui.h"

class ParticleManager :
	public AttributeGui {
public:

	struct ParticlesData {
		std::shared_ptr<std::list<ParticleSingle>> particles;
		std::vector<ParticleInstancingRenderer::ParticleData> forGpuData_;
		bool isAddBlend;

		ParticlesData() {
			particles = std::make_shared<std::list<ParticleSingle>>();
		}
	};

public:

	ParticleManager() = default;
	~ParticleManager();
	ParticleManager(const ParticleManager&) = delete;
	const ParticleManager& operator=(const ParticleManager&) = delete;

	static ParticleManager* GetInstance();

	void Finalize();

	void Init();

	void Update();

	void ParticlesUpdate();

	void PostUpdate();

	void Draw() const;

	void Debug_Gui() override;

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

	// particleの描画を呼び出すレンダラー
	std::unique_ptr<ParticleInstancingRenderer> particleRenderer_;
	// particleを射出するリスト
	std::list<std::unique_ptr<BaseParticles>> emitterList_;

	std::unordered_map<std::string, ParticlesData> particlesMap_;
		
};