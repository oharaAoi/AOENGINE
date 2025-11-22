#include "ProcessedSceneFrame.h"
#include "Engine/WinApp/WinApp.h"

void ProcessedSceneFrame::Finalize() {
	renderResource_->Destroy();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void ProcessedSceneFrame::Init(DxResourceManager* _dxResourceManager) {
	// 最終的に描画させるResourceの作成
	D3D12_RESOURCE_DESC desc{};
	desc.Width = WinApp::sWindowWidth;			// 画面の横幅
	desc.Height = WinApp::sWindowHeight;			// 画面の縦幅
	desc.MipLevels = 1;			// 
	desc.DepthOrArraySize = 1;
	desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	desc.SampleDesc.Count = 1;
	desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

	// HEAPの設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

	// HEAPの設定
	// Resourceの作成
	renderResource_ = _dxResourceManager->CreateResource(ResourceType::Common);
	renderResource_->CreateResource(&desc, &heapProperties, D3D12_HEAP_FLAG_ALLOW_DISPLAY, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

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
	srvDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	renderResource_->CreateSRV(srvDesc);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 描画処理
///////////////////////////////////////////////////////////////////////////////////////////////

void ProcessedSceneFrame::Draw(ID3D12GraphicsCommandList* commandList) {
	commandList->SetGraphicsRootDescriptorTable(0, renderResource_->GetSRV().handleGPU);
	commandList->DrawIndexedInstanced(3, 1, 0, 0, 0);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ sceneをImGuiで描画する
///////////////////////////////////////////////////////////////////////////////////////////////

void ProcessedSceneFrame::DrawScene() {
	if (ImGui::Begin("Game Window", nullptr)) {
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 1.0f);  // アルファの影響を無効化
		ImTextureID textureID = reinterpret_cast<ImTextureID>(static_cast<uint64_t>(renderResource_->GetSRV().handleGPU.ptr));
		availSize_ = ImGui::GetContentRegionAvail();
		// アスペクト比維持したい場合はここで調整
		const float aspect = 16.0f / 9.0f;
		if (availSize_.x / availSize_.y > aspect) {
			// 横が余る → 高さ基準に合わせる
			availSize_.x = availSize_.y * aspect;
		} else {
			// 縦が余る → 横基準に合わせる
			availSize_.y = availSize_.x / aspect;
		}
		ImGui::Image((void*)textureID, availSize_, ImVec2(0, 0), ImVec2(1, 1)); // サイズは適宜調整
		ImGui::PopStyleVar();
	}
}

void ProcessedSceneFrame::TransitionResource(ID3D12GraphicsCommandList* commandList, const D3D12_RESOURCE_STATES& beforState, const D3D12_RESOURCE_STATES& afterState) {
	renderResource_->Transition(commandList, beforState, afterState);
}
