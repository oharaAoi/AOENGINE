#pragma once
#include <list>
#include <memory>
#include "Engine/DirectX/RTV/RenderTarget.h"
#include "Engine/DirectX/Descriptor/DescriptorHeap.h"
// particle
#include "Engine/Render/ParticleInstancingRenderer.h"
#include "Engine/System/ParticleSystem/Emitter/CpuEmitter.h"
#include "Engine/System/ParticleSystem/EffectSystemCamera.h"
#include "Engine/System/Manager/ParticleManager.h"
#include "Engine/Module/Components/Effect/BaseParticles.h"

class ParticleSystemEditor final {
public:

	ParticleSystemEditor() = default;
	~ParticleSystemEditor() = default;

	void Finalize();

	void Init(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, RenderTarget* renderTarget, DescriptorHeap* descriptorHeaps);

	void Update();

	void Draw();

private:		// member method

#ifdef _DEBUG

	/// <summary>
	/// 新たに作成する
	/// </summary>
	void Create();

	/// <summary>
	/// 編集する
	/// </summary>
	void Edit();

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

	// renderer関連 ------------------------------------------

	std::unique_ptr<EffectSystemCamera> camera_ = nullptr;

	std::unique_ptr<ParticleInstancingRenderer> particleRenderer_;

	ParticleManager* particleManager_ = nullptr;

	std::list<std::unique_ptr<BaseParticles>> particles_;
};

