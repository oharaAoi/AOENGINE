#include "GraphicsContext.h"

GraphicsContext* GraphicsContext::GetInstance() {
	static GraphicsContext instance;
	return &instance;
}

void GraphicsContext::Init(WinApp* win, Shader* shaders, int32_t backBufferWidth, int32_t backBufferHeight) {
	dxCommon_ = std::make_unique<DirectXCommon>();
	dxCommon_->Init(win, backBufferWidth, backBufferHeight);

	dxDevice_ = std::make_shared<DirectXDevice>();
	dxCommands_ = std::make_unique<DirectXCommands>();
	descriptorHeap_ = std::make_unique<DescriptorHeap>();
	dxCompiler_ = std::make_unique<DirectXCompiler>();

	renderTarget_ = std::make_unique<RenderTarget>();

	graphicsPipelines_ = std::make_unique<GraphicsPipelines>();
	primitivePipeline_ = std::make_unique<PrimitivePipeline>();

	// 初期化
	dxDevice_->Init(dxCommon_->GetUseAdapter());
	dxCommands_->Init(dxDevice_->GetDevice());
	descriptorHeap_->Init(dxDevice_->GetDevice());
	dxCompiler_->Init();

	dxCommon_->Setting(dxDevice_->GetDevice(), dxCommands_.get(), descriptorHeap_.get(), renderTarget_.get());
	renderTarget_->Init(dxDevice_->GetDevice(), descriptorHeap_.get(), dxCommon_->GetSwapChain().Get(), dxCommands_->GetCommandList());

	graphicsPipelines_->Init(dxDevice_->GetDevice(), dxCompiler_.get());
	primitivePipeline_->Init(dxDevice_->GetDevice(), dxCompiler_.get(), shaders->GetShaderData(Shader::Primitive));

}

void GraphicsContext::Finalize() {
	primitivePipeline_->Finalize();
	graphicsPipelines_->Finalize();

	renderTarget_->Finalize();

	dxCompiler_->Finalize();
	descriptorHeap_->Finalize();
	dxCommands_->Finalize();
	dxDevice_->Finalize();
	dxCommon_->Finalize();
}
