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

class ParticleSystemEditor final {
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

	ParticleSystemEditor() = default;
	~ParticleSystemEditor() = default;

	void Finalize();

	void Init(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, RenderTarget* renderTarget, DescriptorHeap* descriptorHeaps);

	void Update();

	void Draw();

private:		// member method

#ifdef _DEBUG

	void ParticlesUpdate();

	void InputText();

	/// <summary>
	/// 新たに作成する
	/// </summary>
	void Create();

	GpuParticleEmitter* CreateOfGpu();

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
	void Edit();

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
	/// RenderTarget設定
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

	const int32_t kClientWidth_ = 1280;
	const int32_t kClientHeight_ = 720;

	// directX関連 -------------------------------------------

	ID3D12GraphicsCommandList* commandList_;
	RenderTarget* renderTarget_ = nullptr;
	DescriptorHeap* descriptorHeaps_ = nullptr;
	ComPtr<ID3D12Resource> depthStencilResource_ = nullptr;
	DescriptorHandles depthHandle_;

	// renderer関連 ------------------------------------------

	std::unique_ptr<EffectSystemCamera> camera_ = nullptr;

	std::unique_ptr<ParticleInstancingRenderer> particleRenderer_;
	std::unique_ptr<GpuParticleRenderer> gpuParticleRenderer_;

	std::list<std::unique_ptr<BaseParticles>> cpuEmitterList_;
	std::list<std::unique_ptr<GpuParticleEmitter>> gpuEmitterList_;

	std::unordered_map<std::string, ParticlesData> particlesMap_;

	// editer関連 ------------------------------------------
	bool isSave_;
	bool isLoad_;
	bool isGpu_ = false;

	std::string newParticleName_ = "new Particle";
};

