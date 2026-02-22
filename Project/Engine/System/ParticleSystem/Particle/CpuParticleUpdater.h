#pragma once
// c++
#include <string>
#include <unordered_map>
// engine
#include "Engine/System/ParticleSystem/Data/ParticleRuntimeState.h"
#include "Engine/Render/ParticleInstancingRenderer.h"

namespace AOENGINE {

/// <summary>
/// Particleの更新を行うためのクラス
/// </summary>
class CpuParticleUpdater {
public: // コンストラクタ

	CpuParticleUpdater() = default;
	~CpuParticleUpdater();

public:

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// Rendererの更新
	/// </summary>
	/// <param name="renderer"></param>
	void RendererUpdate(ParticleInstancingRenderer* renderer);

	/// <summary>
	/// 追加
	/// </summary>
	/// <param name="name">: 名前</param>
	void Add(const std::string& name);

public:

	const std::shared_ptr<std::list<AOENGINE::ParticleSingle>>& GetParticles(const std::string& name) const;

	void SetRuntimeAddBlend(const std::string& name, bool isAdd);

private:

	std::unordered_map<std::string, ParticleRuntimeState> particlesMap_;

};

}