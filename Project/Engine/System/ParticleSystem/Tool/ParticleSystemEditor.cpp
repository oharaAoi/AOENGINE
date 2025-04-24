#include "ParticleSystemEditor.h"
#include "Engine/Utilities/DrawUtils.h"

void ParticleSystemEditor::Finalize() {
	depthStencilResource_.Reset();
	particleRenderer_.reset();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void ParticleSystemEditor::Init(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, RenderTarget* renderTarget, DescriptorHeap* descriptorHeaps) {
	commandList_ = commandList;
	renderTarget_ = renderTarget;
	descriptorHeaps_ = descriptorHeaps;

	// -------------------------------------------------
	// ↓ 深度バッファの作成
	// -------------------------------------------------
	depthStencilResource_ = CreateDepthStencilTextureResource(device, kClientWidth_, kClientHeight_);
	// DSVの生成
	D3D12_DEPTH_STENCIL_VIEW_DESC desc{};
	desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;

	device->CreateDepthStencilView(depthStencilResource_.Get(), &desc, descriptorHeaps_->GetDescriptorHandle(TYPE_DSV).handleCPU);

	// -------------------------------------------------
	// ↓ Rendererの作成
	// -------------------------------------------------
	particleRenderer_ = std::make_unique<ParticleInstancingRenderer>();
	particleRenderer_->Init(100);

	camera_ = std::make_unique<EffectSystemCamera>();
	camera_->Init();

	emitterList_.push_back(CpuEmitter());
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void ParticleSystemEditor::Update() {
	camera_->Update();

	Edit();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 描画処理
///////////////////////////////////////////////////////////////////////////////////////////////

void ParticleSystemEditor::Draw() {
	PreDraw();

	PostDraw();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ RenderTargetをEffectEditer用にする
///////////////////////////////////////////////////////////////////////////////////////////////

void ParticleSystemEditor::SetRenderTarget() {
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = descriptorHeaps_->GetDSVHeap()->GetCPUDescriptorHandleForHeapStart();
	dsvHandle.ptr += size_t(descriptorHeaps_->GetDescriptorSize()->GetDSV());
	// RenderTargetを指定する
	renderTarget_->SetRenderTarget(commandList_, RenderTargetType::EffectSystem_RenderTarget);
	commandList_->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
	float clearColor[] = { 0.0f / 255, 0.0f / 255, 0.0f / 255.0f, 255.0f };
	// RenderTargetをクリアする
	commandList_->ClearRenderTargetView(renderTarget_->GetRenderTargetRTVHandle(RenderTargetType::EffectSystem_RenderTarget).handleCPU, clearColor, 0, nullptr);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集を行う
///////////////////////////////////////////////////////////////////////////////////////////////

void ParticleSystemEditor::Edit() {
#ifdef _DEBUG

	ImGui::Begin("List");
	// emitterのリスト
	static int selectedEffectIndex = -1;
	int index = 0;
	for (auto it = emitterList_.begin(); it != emitterList_.end(); ++it, ++index) {
		std::string label = "emitter_" + std::to_string(index);

		if (ImGui::Selectable(label.c_str(), selectedEffectIndex == index)) {
			selectedEffectIndex = index;
		}
	}
	ImGui::End();

	ImGui::Begin("Setting");
	// リストから選択されたEmitterを編集
	if (selectedEffectIndex >= 0 && selectedEffectIndex < emitterList_.size()) {
		auto it = emitterList_.begin();
		std::advance(it, selectedEffectIndex);
		(*it).Debug_Gui();
	}
	ImGui::End();
#endif // _DEBUG#endif // _DEBUG
}

void ParticleSystemEditor::PreDraw() {
#ifdef _DEBUG
	SetRenderTarget();

	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));
	ImGui::Begin("ParticleSystemEditor", nullptr,
				 ImGuiWindowFlags_NoTitleBar |
				 ImGuiWindowFlags_NoResize |
				 ImGuiWindowFlags_NoBackground);

	// Grid線描画
	DrawGrid(camera_->GetViewMatrix(), camera_->GetProjectionMatrix());
#endif
}

void ParticleSystemEditor::PostDraw() {
#ifdef _DEBUG
	Engine::SetPSOPrimitive();
	Render::PrimitiveDrawCall();

	// 最後にImGui上でEffectを描画する
	renderTarget_->TransitionResource(commandList_, EffectSystem_RenderTarget, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	ImTextureID textureID2 = reinterpret_cast<ImTextureID>(static_cast<uint64_t>(renderTarget_->GetRenderTargetSRVHandle(RenderTargetType::EffectSystem_RenderTarget).handleGPU.ptr));
	ImGui::SetCursorPos(ImVec2(20, 30)); // 描画位置を設定
	ImGui::Image((void*)textureID2, ImVec2(640.0f, 360.0f)); // サイズは適宜調整

	ImGui::End();
	ImGui::PopStyleColor();
#endif
}

void ParticleSystemEditor::End() {
	renderTarget_->TransitionResource(commandList_, EffectSystem_RenderTarget, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);
}
