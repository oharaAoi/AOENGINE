#pragma once
#include <list>
#include <memory>
#include "Engine/DirectX/RTV/RenderTarget.h"
#include "Engine/DirectX/Descriptor/DescriptorHeap.h"
// particle
#include "Engine/Render/ParticleInstancingRenderer.h"
#include "Engine/Render/GpuParticleRenderer.h"
#include "Engine/System/ParticleSystem/Emitter/GpuParticleEmitter.h"
#include "Engine/System/ParticleSystem/EffectSystemCamera.h"
#include "Engine/System/Manager/ParticleManager.h"
#include "Engine/Module/Components/Effect/BaseParticles.h"

namespace AOENGINE {

/// <summary>
/// Particleを編集するEditor
/// </summary>
class ParticleSystemEditor final {
public:

	struct ParticlesData {
		std::shared_ptr<std::list<AOENGINE::ParticleSingle>> particles;
		std::vector<AOENGINE::ParticleInstancingRenderer::ParticleData> forGpuData_;

		bool isAddBlend = false;
		bool anyParticleAlive = false;

		ParticlesData() {
			particles = std::make_shared<std::list<AOENGINE::ParticleSingle>>();
		}
	};

public:

	ParticleSystemEditor() = default;
	~ParticleSystemEditor() = default;

public:

	/// <summary>
	/// 終了処理
	/// </summary>
	void Finalize();

	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="device"></param>
	/// <param name="commandList"></param>
	/// <param name="renderTarget"></param>
	/// <param name="descriptorHeaps"></param>
	void Init(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, AOENGINE::RenderTarget* renderTarget, AOENGINE::DescriptorHeap* descriptorHeaps);

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();

private:		// member method

#ifdef _DEBUG

	/// <summary>
	/// Particleを更新する
	/// </summary>
	void ParticlesUpdate();

	void InputText();

	/// <summary>
	/// 新たに作成する
	/// </summary>
	void Create();

	AOENGINE::GpuParticleEmitter* CreateOfGpu();

	/// <summary>
	/// リストに追加する
	/// </summary>
	void AddList(const std::string& _name);

	/// <summary>
	/// ファイルを新たに読み込む
	/// </summary>
	void OpenLoadDialog();

	/// <summary>
	/// ファイルを新たに読み込む
	/// </summary>
	json Load(const std::string& filePath);

	/// <summary>
	/// 編集する
	/// </summary>
	void InspectorWindow();

	/// <summary>
	/// SaveDialogを開く
	/// </summary>
	void OpenSaveDialog(const std::string& _name, const json& _jsonData);

	/// <summary>
	/// Saveを行う
	/// </summary>
	/// <param name="directoryPath"></param>
	/// <param name="fileName"></param>
	void Save(const std::string& directoryPath, const std::string& fileName, const json& jsonData);

	/// <summary>
	/// AOENGINE::RenderTarget設定
	/// </summary>
	void SetRenderTarget();

	/// <summary>
	/// 描画前処理
	/// </summary>
	void PreDraw();

	/// <summary>
	/// 描画後処理
	/// </summary>
	void PostDraw();

public:

	/// <summary>
	/// 終了処理
	/// </summary>
	void End();

#endif

private:

	// directX関連 -------------------------------------------

	ID3D12GraphicsCommandList* commandList_;
	AOENGINE::RenderTarget* renderTarget_ = nullptr;
	AOENGINE::DescriptorHeap* descriptorHeaps_ = nullptr;
	ComPtr<ID3D12Resource> depthStencilResource_ = nullptr;
	DescriptorHandles depthHandle_;

	// renderer関連 ------------------------------------------

	std::unique_ptr<EffectSystemCamera> camera_ = nullptr;

	std::unique_ptr<AOENGINE::ParticleInstancingRenderer> particleRenderer_;
	std::unique_ptr<AOENGINE::GpuParticleRenderer> gpuParticleRenderer_;

	std::list<std::unique_ptr<AOENGINE::BaseParticles>> cpuEmitterList_;
	std::list<std::unique_ptr<AOENGINE::GpuParticleEmitter>> gpuEmitterList_;

	std::unordered_map<std::string, ParticlesData> particlesMap_;

	// editer関連 ------------------------------------------
	bool isSave_;
	bool isLoad_;
	bool isGpu_ = false;

	std::string newParticleName_ = "new Particle";
};

}