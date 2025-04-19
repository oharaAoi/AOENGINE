#pragma once
#include <memory>
#include "Engine/PostEffect/Grayscale.h"
#include "Engine/PostEffect/PingPongBuffer.h"
#include "Engine/DirectX/Resource/ShaderResource.h"

/// <summary>
/// postEffectなどを行うクラス
/// </summary>
class PostProcess {
public:

	PostProcess() = default;
	~PostProcess();

	void Finalize();

	void Init(ID3D12Device* device, DescriptorHeap* descriptorHeap);

	void Execute(ID3D12GraphicsCommandList* commandList, ShaderResource* shaderResource);

	void Copy(ID3D12GraphicsCommandList* commandList, ShaderResource* shaderResource);

	void PostCopy(ID3D12GraphicsCommandList* commandList, ShaderResource* shaderResource);

private:

	std::unique_ptr<PingPongBuffer> pingPongBuff_;

	std::unique_ptr<Grayscale> grayscale_;

};