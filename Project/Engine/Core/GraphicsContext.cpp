#include "GraphicsContext.h"

using namespace AOENGINE;

GraphicsContext* AOENGINE::GraphicsContext::GetInstance() {
	static AOENGINE::GraphicsContext instance;
	return &instance;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void GraphicsContext::Init(WinApp* _win, int32_t _backBufferWidth, int32_t _backBufferHeight) {
	// dxcommonの初期化
	dxCommon_ = std::make_unique<DirectXCommon>();
	dxCommon_->Init(_win, _backBufferWidth, _backBufferHeight);

	// ----------------------
	// ↓ 定義処理
	// ----------------------
	dxDevice_ = std::make_shared<DirectXDevice>();
	dxCommands_ = std::make_unique<DirectXCommands>();
	descriptorHeap_ = std::make_unique<DescriptorHeap>();
	dxCompiler_ = std::make_unique<DirectXCompiler>();
	renderTarget_ = std::make_unique<RenderTarget>();
	graphicsPipelines_ = std::make_unique<GraphicsPipelines>();
	primitivePipeline_ = std::make_unique<PrimitivePipeline>();
	dxResourceManager_ = std::make_unique<AOENGINE::DxResourceManager>();

	// ----------------------
	// ↓ 初期化処理
	// ----------------------
	dxDevice_->Init(dxCommon_->GetUseAdapter());
	dxCommands_->Init(dxDevice_->GetDevice());
	descriptorHeap_->Init(dxDevice_->GetDevice());
	dxCompiler_->Init();
	dxResourceManager_->Init(dxDevice_->GetDevice(), descriptorHeap_.get());

	dxCommon_->Setting(dxDevice_->GetDevice(), dxCommands_.get(), descriptorHeap_.get(), renderTarget_.get(), dxResourceManager_.get());
	renderTarget_->Init(dxDevice_->GetDevice(), descriptorHeap_.get(), dxCommon_->GetSwapChain().Get(), dxCommands_->GetCommandList(), dxResourceManager_.get());

	graphicsPipelines_->Init(dxDevice_->GetDevice(), dxCompiler_.get());
	primitivePipeline_->Init(dxDevice_->GetDevice(), dxCompiler_.get());

}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 終了処理
///////////////////////////////////////////////////////////////////////////////////////////////

void GraphicsContext::Finalize() {
	primitivePipeline_->Finalize();
	graphicsPipelines_->Finalize();

	renderTarget_->Finalize();

	dxCompiler_->Finalize();
	dxResourceManager_->Finalize();
	descriptorHeap_->Finalize();
	dxCommands_->Finalize();
	dxDevice_->Finalize();
	dxCommon_->Finalize();
}

AOENGINE::DxResource* GraphicsContext::CreateDxResource(ResourceType _type) {
	return dxResourceManager_->CreateResource(_type);
}
