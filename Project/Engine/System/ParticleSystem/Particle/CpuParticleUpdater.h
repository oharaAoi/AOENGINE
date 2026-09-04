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
	/// 終了処理
	/// </summary>
	void Finalize();

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	// 新規生成の前に、このフレームで寿命を迎えるParticleを回収する
	void RemoveDeadParticles();

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
	void Remove(const std::string& name);

public:

	const std::shared_ptr<std::list<AOENGINE::ParticleSingle>>& GetParticles(const std::string& name) const;

	void SetRuntimeBlendMode(const std::string& name, uint32_t blendType);

private:

	std::unordered_map<std::string, ParticleRuntimeState> particlesMap_;

};

}
