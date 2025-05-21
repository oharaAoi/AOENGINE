#pragma once
#include "Engine/WinApp/WinApp.h"
#include "Engine/Utilities/Shader.h"

#include "Engine/DirectX/DirectXCommon/DirectXCommon.h"
#include "Engine/DirectX/DirectXDevice/DirectXDevice.h"
#include "Engine/DirectX/DirectXCommands/DirectXCommands.h"
#include "Engine/DirectX/Descriptor/DescriptorHeap.h"
#include "Engine/DirectX/RTV/RenderTarget.h"
#include "Engine/DirectX/DirectXCompiler/DirectXCompiler.h"
#include "Engine/DirectX/Pipeline/PipelineGroup/GraphicsPipelines.h"
#include "Engine/DirectX/Pipeline/PipelineGroup/PrimitivePipeline.h"

/// <summary>
/// DirectXの内容をまとめたクラス
/// </summary>
class GraphicsContext {
public:

	GraphicsContext() = default;
	~GraphicsContext() = default;
	GraphicsContext(const GraphicsContext&) = delete;
	const GraphicsContext& operator=(const GraphicsContext&) = delete;

	static GraphicsContext* GetInstance();

	void Init(WinApp* win, Shader* shaders, int32_t backBufferWidth, int32_t backBufferHeight);

	void Finalize();

public:

	DirectXCommon* GetDxCommon() { return dxCommon_.get(); }

	ID3D12Device* GetDevice() { return dxDevice_->GetDevice(); }

	ID3D12GraphicsCommandList* GetCommandList() { return dxCommands_->GetCommandList(); }

	DirectXCompiler* GetDxCompiler() { return dxCompiler_.get(); }

	DescriptorHeap* GetDxHeap() { return descriptorHeap_.get(); }

	GraphicsPipelines* GetGraphicsPipeline() { return graphicsPipelines_.get(); }

	PrimitivePipeline* GetPrimitivePipeline() { return primitivePipeline_.get(); }

	RenderTarget* GetRenderTarget() { return renderTarget_.get(); }

private :

	// dxCommon
	std::unique_ptr<DirectXCommon> dxCommon_ = nullptr;
	// dxDevice
	std::shared_ptr<DirectXDevice> dxDevice_ = nullptr;
	// descriptorHeap
	std::unique_ptr<DescriptorHeap> descriptorHeap_ = nullptr;
	// dxCommand
	std::unique_ptr<DirectXCommands> dxCommands_ = nullptr;
	// renderTarget
	std::shared_ptr<RenderTarget> renderTarget_ = nullptr;
	// dxCompiler
	std::unique_ptr<DirectXCompiler> dxCompiler_ = nullptr;
	// pipeline
	std::unique_ptr<GraphicsPipelines> graphicsPipelines_ = nullptr;
	std::unique_ptr<PrimitivePipeline> primitivePipeline_ = nullptr;

};

