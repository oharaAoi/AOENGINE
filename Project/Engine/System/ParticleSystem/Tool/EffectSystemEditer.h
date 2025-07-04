#pragma once
#include "Engine/DirectX/RTV/RenderTarget.h"
#include "Engine/DirectX/Descriptor/DescriptorHeap.h"
#include "Engine/DirectX/DirectXCommands/DirectXCommands.h"
#include "Engine/DirectX/DirectXCommon/DirectXCommon.h"
#include "Engine/System/ParticleSystem/ParticleField.h"
#include "Engine/System/ParticleSystem/EffectSystemCamera.h"
#include "Engine/System/ParticleSystem/GpuEffect.h"
#include "Engine/System/ParticleSystem/Particle/GpuParticles.h"
#include "Game/WorldObject/Skydome.h"

/// <summary>
/// effectを作成する用のクラス
/// </summary>
class EffectSystemEditer {
public:

#ifdef _DEBUG

	EffectSystemEditer(RenderTarget* renderTarget, DescriptorHeap* descriptorHeaps, ID3D12GraphicsCommandList* dxCommandList, ID3D12Device* device);
	~EffectSystemEditer();

	void Finalize();
	void Init(RenderTarget* renderTarget, DescriptorHeap* descriptorHeaps, ID3D12GraphicsCommandList* dxCommandList, ID3D12Device* device);
	void Update();
	void Draw() const;

	void Import();

	void Begin();
	void End();

	void Debug_Gui();

	void CreateEmitter(const std::string& emitterName);

public:

	const bool GetIsFocused() const { return isFocused_; }

private:

	void EditEmitter();

	void EditEffect();

	void SaveEffect();

	void LoadEffect();

private:

	int32_t kClientWidth_ = 1280;
	int32_t kClientHeight_ = 720;

	std::unique_ptr<EffectSystemCamera> effectSystemCamera_ = nullptr;

	// ----------- Editerで使用する変数 ----------- //
	// RenderTarget
	RenderTarget* renderTarget_ = nullptr;
	// descriptorHeap
	DescriptorHeap* descriptorHeaps_ = nullptr;
	// commands
	ID3D12GraphicsCommandList* dxCmdList_ = nullptr;
	// device
	ID3D12Device* device_ = nullptr;
	// dsv
	ComPtr<ID3D12Resource> depthStencilResource_ = nullptr;

	int createShape_;

	bool isFocused_;	// windowが選択されているか

	// ----------- field ----------- //
	// filed
	std::unique_ptr<ParticleField> particleField_ = nullptr;

	//std::unique_ptr<Skydome> skydome_;

	// ----------- effect ----------- //

	std::unique_ptr<GpuParticles> gpuParticles_ = nullptr;
	std::list<std::unique_ptr<GpuEmitter>> gpuEmitterList_;

	// ----------- Saveに使用 ----------- //
	const std::string kDirectoryPath_ = "./Game/Resources/Effects/";
	std::vector<std::string> emitterNames_;

	std::vector<std::string> effectEmitterNames_;

	// effect関連
	std::string newEffectName_;
	char inputNewEffectNameBuffer_[64];

	// ----------- loadに使用 ----------- //
	std::vector<std::string> effectFiles_;
	std::string importEffectName_;
	uint32_t importEffectIndex_;

	std::vector<std::string> emitterFiles_;
	std::string importFileName_;
	uint32_t importIndex_;

#endif // _DEBUG
};
