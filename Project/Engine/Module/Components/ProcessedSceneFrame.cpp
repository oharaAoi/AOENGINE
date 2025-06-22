#include "ProcessedSceneFrame.h"

void ProcessedSceneFrame::Finalize() {
	renderResource_->Finalize();
}

void ProcessedSceneFrame::Init(ID3D12Device* device, DescriptorHeap* dxHeap) {
	// 最終的に描画させるResourceの作成
	D3D12_RESOURCE_DESC desc{};
	desc.Width = kWindowWidth_;			// 画面の横幅
	desc.Height = kWindowHeight_;		// 画面の縦幅
	desc.MipLevels = 1;			// 
	desc.DepthOrArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

	// HEAPの設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

	// HEAPの設定
	// Resourceの作成
	renderResource_ = std::make_unique<ShaderResource>();
	renderResource_->Init(device, dxHeap, desc, &heapProperties, D3D12_HEAP_FLAG_ALLOW_DISPLAY, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	// ------------------------------------------------------------
	// UAVの設定
	D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
	uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
	uavDesc.Format = renderResource_->GetResource()->GetDesc().Format;
	// 作成
	renderResource_->CreateUAV(uavDesc);

	// ------------------------------------------------------------
	// SRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	renderResource_->CreateSRV(srvDesc);
}

void ProcessedSceneFrame::Draw(ID3D12GraphicsCommandList* commandList) {
	commandList->SetGraphicsRootDescriptorTable(0, renderResource_->GetSRV().handleGPU);
	commandList->DrawIndexedInstanced(3, 1, 0, 0, 0);
}

void ProcessedSceneFrame::DrawGui() {
	ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 1.0f);  // アルファの影響を無効化
	ImTextureID textureID = reinterpret_cast<ImTextureID>(static_cast<uint64_t>(renderResource_->GetSRV().handleGPU.ptr));
	ImGui::SetCursorPos(ImVec2(20, 60)); // 描画位置を設定
	ImGui::Image((void*)textureID, ImVec2(640.0f, 360.0f), ImVec2(0, 0), ImVec2(1, 1)); // サイズは適宜調整
	ImGui::PopStyleVar();
}

void ProcessedSceneFrame::TransitionResource(ID3D12GraphicsCommandList* commandList, const D3D12_RESOURCE_STATES& beforState, const D3D12_RESOURCE_STATES& afterState) {
	renderResource_->Transition(commandList, beforState, afterState);
}
