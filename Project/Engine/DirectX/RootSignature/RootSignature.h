#pragma once
#include <unordered_map>
#include "Engine/Utilities/Convert.h"
#include "Engine/DirectX/Utilities/DirectXUtils.h"
#include "Engine/Utilities/Debug.h"
#include "Engine/DirectX/RootSignature/RootSignatureBuilder.h"

enum class RootSignatureType {
	Normal,
	NormalEnviroment,
	TextureLess,
	Primitive,
	Particle,
	Sprite,
	PBR,
	Skybox,
	TextureBlend,

	ProcessedScene,	// ポストプロセス
	RadialBlur,
	Vignette,
	ComputeShader,
	CsSkinning,
	ComputeShaderBlend,
	CSReultRenderBlend,

	GpuParticleInit,
	GpuParticleUpdate,
	GpuParticleEnd,
	EmitGpuParticle,
};

class RootSignature {
public:

	RootSignature();
	~RootSignature();

	void Initialize(ID3D12Device* device, const RootSignatureType& type);

	void Finalize();

	ComPtr<ID3D12RootSignature> CreateNormal();
	ComPtr<ID3D12RootSignature> CreateNormalEnviroment();
	ComPtr<ID3D12RootSignature> CreateTextureless();
	ComPtr<ID3D12RootSignature> CreatePrimitive();
	ComPtr<ID3D12RootSignature> CreateParticle();
	ComPtr<ID3D12RootSignature> CreateSprite();
	ComPtr<ID3D12RootSignature> CreatePBR();
	ComPtr<ID3D12RootSignature> CreateSkybox();
	ComPtr<ID3D12RootSignature> CreateTextureBlend();

	ComPtr<ID3D12RootSignature> CreateComputeShader();
	ComPtr<ID3D12RootSignature> CreateCsSkinnig();
	ComPtr<ID3D12RootSignature> CreateBlendShader();
	ComPtr<ID3D12RootSignature> CreateResultRender();
	ComPtr<ID3D12RootSignature> CreateProcessedScene();
	ComPtr<ID3D12RootSignature> CreateRadialBlur();
	ComPtr<ID3D12RootSignature> CreateVignette();

	ComPtr<ID3D12RootSignature> CreateGpuParticleInit();
	ComPtr<ID3D12RootSignature> CreateGpuParticleUpdate();
	ComPtr<ID3D12RootSignature> CreateGpuParticleEnd();
	ComPtr<ID3D12RootSignature> CreateEmitGpuParticle();

public:

	using FunctionPointer = ComPtr<ID3D12RootSignature>(RootSignature::*)();
	std::unordered_map<RootSignatureType, FunctionPointer> functionMap_ = {
		// graphics
		{RootSignatureType::Normal, &RootSignature::CreateNormal},
		{RootSignatureType::NormalEnviroment, &RootSignature::CreateNormalEnviroment},
		{RootSignatureType::TextureLess, &RootSignature::CreateTextureless},
		{RootSignatureType::Primitive, &RootSignature::CreatePrimitive},
		{RootSignatureType::Particle, &RootSignature::CreateParticle},
		{RootSignatureType::Sprite, &RootSignature::CreateSprite},
		{RootSignatureType::PBR, &RootSignature::CreatePBR},
		{RootSignatureType::Skybox, &RootSignature::CreateSkybox},
		{RootSignatureType::TextureBlend, &RootSignature::CreateTextureBlend},

		{RootSignatureType::ProcessedScene, &RootSignature::CreateProcessedScene},
		{RootSignatureType::RadialBlur, &RootSignature::CreateRadialBlur},
		{RootSignatureType::Vignette, &RootSignature::CreateVignette},
		// CS
		{RootSignatureType::ComputeShader, &RootSignature::CreateComputeShader},
		{RootSignatureType::CsSkinning, &RootSignature::CreateCsSkinnig},
		{RootSignatureType::ComputeShaderBlend, &RootSignature::CreateBlendShader},
		{RootSignatureType::CSReultRenderBlend, &RootSignature::CreateResultRender},
		// Particle
		{RootSignatureType::GpuParticleInit, &RootSignature::CreateGpuParticleInit},
		{RootSignatureType::GpuParticleUpdate, &RootSignature::CreateGpuParticleUpdate},
		{RootSignatureType::GpuParticleEnd, &RootSignature::CreateGpuParticleEnd},
		{RootSignatureType::EmitGpuParticle, &RootSignature::CreateEmitGpuParticle},
	};

	ComPtr<ID3D12RootSignature> Create(const RootSignatureType& type);

	D3D12_STATIC_SAMPLER_DESC CreateSampler(D3D12_TEXTURE_ADDRESS_MODE mode);

public:

	ID3D12RootSignature* GetRootSignature() { return rootSignature_.Get(); }

private:

	// rootSignature
	ComPtr<ID3D12RootSignature> rootSignature_ = nullptr;

	RootSignatureBuilder builder_;

	ID3D12Device* device_ = nullptr;
};