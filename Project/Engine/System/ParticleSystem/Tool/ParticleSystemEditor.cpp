#include "ParticleSystemEditor.h"
#include "Engine/Utilities/DrawUtils.h"

void ParticleSystemEditor::Finalize() {
	depthStencilResource_.Reset();
	particleRenderer_.reset();
	particles_.clear();
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

	particleManager_ = ParticleManager::GetInstance();
	particleManager_->Init();

	camera_ = std::make_unique<EffectSystemCamera>();
	camera_->Init();

}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void ParticleSystemEditor::Update() {
	for (auto& particle : particles_) {
		particle->Update(camera_->GetRotate());
		particleRenderer_->Update(particle->GetName(), particle->GetData());
	}
	camera_->Update();

	particleRenderer_->PostUpdate();
	particleRenderer_->SetView(camera_->GetViewMatrix() * camera_->GetProjectionMatrix(), Matrix4x4::MakeUnit());

#ifdef _DEBUG
	Create();
	Edit();
#endif // _DEBUG
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 描画処理
///////////////////////////////////////////////////////////////////////////////////////////////

void ParticleSystemEditor::Draw() {
	PreDraw();
	particleRenderer_->Draw(commandList_);
	PostDraw();
}

#ifdef _DEBUG
///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 生成する
///////////////////////////////////////////////////////////////////////////////////////////////

void ParticleSystemEditor::Create() {
	ImGui::Begin("Create Window");
	static std::string newEffectName = "new Particles";
	char buffer[128];
	strncpy_s(buffer, sizeof(buffer), newEffectName.c_str(), _TRUNCATE);
	buffer[sizeof(buffer) - 1] = '\0'; // 安全のため null 終端

	if (ImGui::InputText("Effect Name", buffer, sizeof(buffer))) {
		newEffectName = buffer;
	}

	if (ImGui::Button("Create")) {
		auto& newParticle = particles_.emplace_back(std::make_unique<BaseParticles>());
		newParticle->Init(newEffectName, true);
		particleRenderer_->AddParticle(newParticle->GetName(),
									   newParticle->GetGeometryObject()->GetMesh(),
									   newParticle->GetGeometryObject()->GetMaterial(),
									   newParticle->GetIsAddBlend());
	}

	ImGui::End();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集を行う
///////////////////////////////////////////////////////////////////////////////////////////////

void ParticleSystemEditor::Edit() {
	// 編集したいParticleの指定を行う
	ImGui::Begin("List");
	static BaseParticles* particles = nullptr;
	static std::string openNode = "";
	for (auto& it : particles_) {
		BaseParticles* ptr = it.get();
		if (ImGui::Selectable(ptr->GetName().c_str(), particles == ptr)) {
			particles = it.get();
			openNode = "";  // 他のノードを閉じる
		}
	}
	ImGui::End();

	// 指定されたParticleの編集を行う
	ImGui::Begin("Setting");
	if (particles != nullptr) {
		particles->Debug_Gui();
	}
	ImGui::End();
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
// ↓ 描画前処理
///////////////////////////////////////////////////////////////////////////////////////////////

void ParticleSystemEditor::PreDraw() {
	SetRenderTarget();
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));
	ImGui::Begin("ParticleSystemEditor", nullptr,
				 ImGuiWindowFlags_NoTitleBar |
				 ImGuiWindowFlags_NoResize |
				 ImGuiWindowFlags_NoBackground);

	// Grid線描画
	DrawGrid(camera_->GetViewMatrix(), camera_->GetProjectionMatrix());
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 描画後処理
///////////////////////////////////////////////////////////////////////////////////////////////

void ParticleSystemEditor::PostDraw() {
	Engine::SetPSOPrimitive();
	Render::PrimitiveDrawCall();

	// 最後にImGui上でEffectを描画する
	renderTarget_->TransitionResource(commandList_, EffectSystem_RenderTarget, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	ImTextureID textureID2 = reinterpret_cast<ImTextureID>(static_cast<uint64_t>(renderTarget_->GetRenderTargetSRVHandle(RenderTargetType::EffectSystem_RenderTarget).handleGPU.ptr));
	ImGui::SetCursorPos(ImVec2(20, 30)); // 描画位置を設定
	ImGui::Image((void*)textureID2, ImVec2(640.0f, 360.0f)); // サイズは適宜調整

	ImGui::End();
	ImGui::PopStyleColor();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 終了処理
///////////////////////////////////////////////////////////////////////////////////////////////

void ParticleSystemEditor::End() {
	renderTarget_->TransitionResource(commandList_, EffectSystem_RenderTarget, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);
}

#endif // _DEBUG