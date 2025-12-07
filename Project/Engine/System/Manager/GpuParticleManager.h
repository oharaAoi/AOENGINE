#pragma once
#include <string>
#include <memory>
#include <list>
#include "Engine/Render/GpuParticleRenderer.h"
#include "Engine/System/ParticleSystem/Emitter/GpuParticleEmitter.h"
#include "Engine/System/ParticleSystem/Field/GpuParticleField.h"
#include "Engine/Module/Components/Attribute/AttributeGui.h"

namespace AOENGINE {

/// <summary>
/// gpuParticleを管理しているクラス
/// </summary>
class GpuParticleManager :
	public AOENGINE::AttributeGui {
public:

	GpuParticleManager() = default;
	~GpuParticleManager();
	GpuParticleManager(const GpuParticleManager&) = delete;
	const GpuParticleManager& operator=(const GpuParticleManager&) = delete;

	static AOENGINE::GpuParticleManager* GetInstance();

public:

	// 終了処理
	void Finalize();
	// 初期化処理
	void Init();
	// 更新処理
	void Update();
	// 描画処理
	void Draw() const;
	// 編集処理
	void Debug_Gui() override;

	/// <summary>
	/// Emitterの作成
	/// </summary>
	/// <param name="particlesFile">: emitterのファイル名</param>
	/// <returns></returns>
	GpuParticleEmitter* CreateEmitter(const std::string& particlesFile);

	/// <summary>
	/// Fieldの作成
	/// </summary>
	/// <param name="particlesFile">: 影響を受けるファイル名</param>
	/// <returns></returns>
	GpuParticleField* CreateField(const std::string& particlesFile);

	/// <summary>
	/// Emitterの削除
	/// </summary>
	/// <param name="_emitter"></param>
	void DeleteEmitter(GpuParticleEmitter* _emitter);

private:

	void AddEmitter(GpuParticleEmitter* _emitter);

private:

	std::unique_ptr<AOENGINE::GpuParticleRenderer> renderer_;

	std::list<std::unique_ptr<GpuParticleEmitter>> emitterList_;

	std::list<std::unique_ptr<GpuParticleField>> fileds_;

	const uint32_t instance_ = 1280000;

};

}