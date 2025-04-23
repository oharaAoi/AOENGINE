#pragma once
#include <list>
#include <memory>
#include "Engine/DirectX/RTV/RenderTarget.h"
#include "Engine/DirectX/Descriptor/DescriptorHeap.h"
// particle
#include "Engine/Render/ParticleInstancingRenderer.h"
#include "Engine/System/ParticleSystem/Emitter/CpuEmitter.h"
#include "Engine/System/ParticleSystem/EffectSystemCamera.h"

class ParticleSystemEditor final {
public:

	ParticleSystemEditor() = default;
	~ParticleSystemEditor() = default;

	void Finalize();

	void Init(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, RenderTarget* renderTarget, DescriptorHeap* descriptorHeaps);

	void Update();

	void Draw();

	void End();

private:		// member method

	/// <summary>
	/// RenderTarget設定
	/// </summary>
	void SetRenderTarget();

	/// <summary>
	/// 編集する
	/// </summary>
	void Edit();

	void PreDraw();

	void PostDraw();

private:

	const int32_t kClientWidth_ = 1280;
	const int32_t kClientHeight_ = 720;

	// directX関連 -------------------------------------------

	ID3D12GraphicsCommandList* commandList_;
	RenderTarget* renderTarget_ = nullptr;
	DescriptorHeap* descriptorHeaps_ = nullptr;
	ComPtr<ID3D12Resource> depthStencilResource_ = nullptr;

	// renderer関連 ------------------------------------------

	std::unique_ptr<ParticleInstancingRenderer> particleRenderer_;

	std::unique_ptr<EffectSystemCamera> camera_ = nullptr;

	// Emitter関連 --------------------------------------------
	std::list<CpuEmitter> emitterList_;

};

