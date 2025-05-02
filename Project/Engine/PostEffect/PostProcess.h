#pragma once
#include <memory>
#include "Engine/PostEffect/Grayscale.h"
#include "Engine/PostEffect/RadialBlur.h"
#include "Engine/PostEffect/GlitchNoise.h"
#include "Engine/PostEffect/PingPongBuffer.h"
#include "Engine/DirectX/Resource/ShaderResource.h"
#include "Engine/Components/Attribute/AttributeGui.h"

/// <summary>
/// postEffectなどを行うクラス
/// </summary>
class PostProcess :
	public AttributeGui {
public:

	PostProcess() = default;
	~PostProcess();

	void Finalize();

	void Init(ID3D12Device* device, DescriptorHeap* descriptorHeap);

	void Execute(ID3D12GraphicsCommandList* commandList, ShaderResource* shaderResource);

	void Copy(ID3D12GraphicsCommandList* commandList, ShaderResource* shaderResource);

	void PostCopy(ID3D12GraphicsCommandList* commandList, ShaderResource* shaderResource);

#ifdef _DEBUG
	void Debug_Gui() override;
#endif

private:

	std::unique_ptr<PingPongBuffer> pingPongBuff_;

	std::unique_ptr<Grayscale> grayscale_;
	std::unique_ptr<RadialBlur> radialBlur_;
	std::unique_ptr<GlitchNoise> glitchNoise_;

};