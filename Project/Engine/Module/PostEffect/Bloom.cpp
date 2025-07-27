#include "Bloom.h"
#include "Engine.h"
#include "Engine/Core/GraphicsContext.h"

Bloom::~Bloom() {
	sceneBuffer_->Finalize();
}

void Bloom::Init() {
	GraphicsContext* graphicsCtx = GraphicsContext::GetInstance();
	
	// ブルームに必要なクラスの初期化を行う
	brightnessBuffer_ = std::make_unique<BrightnessThreshold>();
	brightnessBuffer_->Init();
	blurWidthBuffer_ = std::make_unique<GaussianBlurWidth>();
	blurWidthBuffer_->Init();
	blurHeightBuffer_ = std::make_unique<GaussianBlurHeight>();
	blurHeightBuffer_->Init();

	// ブルームの情報
	settingBuffer_ = std::make_unique<DxResource>();
	settingBuffer_->Init(graphicsCtx->GetDevice(), graphicsCtx->GetDxHeap(), ResourceType::COMMON);
	settingBuffer_->CreateResource(sizeof(BloomSettings));
	settingBuffer_->GetResource()->Map(0, nullptr, reinterpret_cast<void**>(&setting_));
	
	setting_->bloomIntensity = 1.8f;

	// ブルームを実行するのに必要なResourceを作詞絵する
	// resourceの設定
	D3D12_RESOURCE_DESC desc{};
	desc.Width = kWindowWidth_;			// 画面の横幅
	desc.Height = kWindowHeight_;			// 画面の縦幅
	desc.MipLevels = 1;			// 
	desc.DepthOrArraySize = 1;
	desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	desc.SampleDesc.Count = 1;
	desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	
	// HEAPの設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

	// SRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	sceneBuffer_ = std::make_unique<DxResource>();
	sceneBuffer_->Init(graphicsCtx->GetDevice(), graphicsCtx->GetDxHeap(), ResourceType::COMMON);
	sceneBuffer_->CreateResource(&desc, &heapProperties, D3D12_HEAP_FLAG_ALLOW_DISPLAY, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	sceneBuffer_->CreateSRV(srvDesc);

	device_ = graphicsCtx->GetDevice();
	dxHeap_ = graphicsCtx->GetDxHeap();
	pingPongBuff_ = std::make_unique<PingPongBuffer>();
	pingPongBuff_->Init(device_, dxHeap_);

}

void Bloom::SetCommand(ID3D12GraphicsCommandList* commandList, DxResource* pingResource) {
	// Sceneの状態を保存しておく
	pingResource->Transition(commandList, D3D12_RESOURCE_STATE_COPY_SOURCE);
	sceneBuffer_->Transition(commandList, D3D12_RESOURCE_STATE_COPY_DEST);
	commandList->CopyResource(sceneBuffer_->GetResource(), pingResource->GetResource());
	// copyの状態から戻す
	pingResource->Transition(commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	sceneBuffer_->Transition(commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	// renderTargetを設定
	pingPongBuff_->SetRenderTarget(commandList, BufferType::PONG, depthHandle_);

	// 輝度の抽出
	brightnessBuffer_->SetCommand(commandList, pingResource);
	pingPongBuff_->Swap(commandList);
	pingPongBuff_->SetRenderTarget(commandList, BufferType::PONG, depthHandle_);

	// ブラーをかける(横)
	blurWidthBuffer_->SetCommand(commandList, pingPongBuff_->GetPingResource());
	pingPongBuff_->Swap(commandList);
	pingPongBuff_->SetRenderTarget(commandList, BufferType::PONG, depthHandle_);

	// ブラーをかける(縦)
	blurHeightBuffer_->SetCommand(commandList, pingPongBuff_->GetPingResource());
	pingPongBuff_->Swap(commandList);
	pingPongBuff_->SetRenderTarget(commandList, BufferType::PONG, depthHandle_);

	// textureの合成を行いpongResourceに書き込む
	postProcessResource_->SetRenderTarget(commandList, BufferType::PONG, depthHandle_);
	Engine::SetPipeline(PSOType::ProcessedScene, "PostProcess_Bloom.json");
	Pipeline* pso = Engine::GetLastUsedPipeline();
	UINT index = pso->GetRootSignatureIndex("gSceneTexture");
	commandList->SetGraphicsRootDescriptorTable(index, sceneBuffer_->GetSRV().handleGPU);
	index = pso->GetRootSignatureIndex("gBloomTexture");
	commandList->SetGraphicsRootDescriptorTable(index, pingPongBuff_->GetPingResource()->GetSRV().handleGPU);
	index = pso->GetRootSignatureIndex("gBloomSettings");
	commandList->SetGraphicsRootConstantBufferView(index, settingBuffer_->GetResource()->GetGPUVirtualAddress());
	commandList->DrawIndexedInstanced(3, 1, 0, 0, 0);

}

void Bloom::CheckBox() {
	ImGui::Checkbox("Bloom###Bloom_Checkbox", &isEnable_);
}

void Bloom::Debug_Gui() {
	if (ImGui::CollapsingHeader("Bloom###Bloom_Header")) {
		brightnessBuffer_->Debug_Gui();
		blurWidthBuffer_->Debug_Gui();
		blurHeightBuffer_->Debug_Gui();
		ImGui::DragFloat("bloomIntensity", &setting_->bloomIntensity, 0.1f);
	}
}