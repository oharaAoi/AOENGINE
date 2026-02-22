#include "ParticleSystemEditor.h"
#include "Engine/System/Manager/ImGuiManager.h"
#include "Engine/Engine.h"
#include "Engine/Render.h"
#include "Engine/Utilities/DrawUtils.h"
#include "Engine/System/Editor/Window/EditorWindows.h"
#include "Engine/Lib/GameTimer.h"
#include <iostream>
#include <fstream>

using namespace AOENGINE;

void ParticleSystemEditor::Finalize() {
	descriptorHeaps_->FreeSRV(depthHandle_.assignIndex_);
	depthStencilResource_.Reset();
	particleRenderer_.reset();
	cpuEmitterList_.clear();
	gpuEmitterList_.clear();
	gpuParticleRenderer_.reset();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void ParticleSystemEditor::Init(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, AOENGINE::RenderTarget* renderTarget, AOENGINE::DescriptorHeap* descriptorHeaps) {
	commandList_ = commandList;
	renderTarget_ = renderTarget;
	descriptorHeaps_ = descriptorHeaps;
	pDevice_ = device;

	// -------------------------------------------------
	// ↓ 深度バッファの作成
	// -------------------------------------------------
	depthStencilResource_ = CreateDepthStencilTextureResource(device, WinApp::sClientWidth, WinApp::sClientHeight);
	// DSVの生成
	D3D12_DEPTH_STENCIL_VIEW_DESC desc{};
	desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;

	depthHandle_ = descriptorHeaps_->AllocateDSV();
	device->CreateDepthStencilView(depthStencilResource_.Get(), &desc, depthHandle_.handleCPU);

	// -------------------------------------------------
	// ↓ Rendererの作成
	// -------------------------------------------------
	particleRenderer_ = std::make_unique<ParticleInstancingRenderer>();
	particleRenderer_->Init(51600);

	gpuParticleRenderer_ = std::make_unique<GpuParticleRenderer>();
	gpuParticleRenderer_->Init(10240);

	camera_ = std::make_unique<EffectSystemCamera>();
	camera_->Init();

	// -------------------------------------------------
	// ↓ Editor関連
	// -------------------------------------------------
	isSave_ = false;
	isLoad_ = false;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void ParticleSystemEditor::Update() {
#ifdef _DEBUG
	// カメラの更新
	camera_->Update();

	// Emitterの更新
	for (auto& emitter : cpuEmitterList_) {
		if (emitter->GetChangeMesh()) {
			emitter->ChangeMesh();
			particleRenderer_->ChangeMesh(emitter->GetName(), emitter->GetMesh());
		}
		emitter->Update();
	}

	for (auto& emitter : gpuEmitterList_) {
		emitter->Update();
	}

	// particleの更新
	particleUpdater_.Update();

	// particleをRendererに送る
	gpuParticleRenderer_->SetView(camera_->GetViewMatrix() * camera_->GetProjectionMatrix(), camera_->GetBillBordMatrix());
	gpuParticleRenderer_->Update();

	particleRenderer_->SetView(camera_->GetViewMatrix() * camera_->GetProjectionMatrix(), AOENGINE::Render::GetProjection2D(), camera_->GetBillBordMatrix());
	particleUpdater_.RendererUpdate(particleRenderer_.get());

	particleRenderer_->PostUpdate();

#endif // _DEBUG
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 描画処理
///////////////////////////////////////////////////////////////////////////////////////////////

void ParticleSystemEditor::Draw() {
#ifdef _DEBUG
	
	PreDraw();
	particleRenderer_->Draw(commandList_);
	gpuParticleRenderer_->Draw();

	for (auto& emitter : gpuEmitterList_) {
		emitter->DrawShape();
	}

	PostDraw();
#endif // _DEBUG
}

#ifdef _DEBUG

void ParticleSystemEditor::InputText() {
	ImGui::Begin("Create Window");
	char buffer[128];
	strncpy_s(buffer, sizeof(buffer), newParticleName_.c_str(), _TRUNCATE);
	buffer[sizeof(buffer) - 1] = '\0'; // 安全のため null 終端

	if (ImGui::InputText("Effect Name", buffer, sizeof(buffer))) {
		newParticleName_ = buffer;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 生成する
///////////////////////////////////////////////////////////////////////////////////////////////

void ParticleSystemEditor::Create() {
	// createする
	ImGui::Checkbox("isGpu", &isGpu_);
	if (ImGui::Button("Create")) {
		if (isGpu_) {
			CreateOfGpu();
		} else {
			AddList(newParticleName_);
		}
	}

	// 読み込みから行う
	if (ImGui::Button("Load")) {
		isLoad_ = true;
	}

	if (isLoad_) {
		OpenLoadDialog();
	}

	ImGui::End();
}

GpuParticleEmitter* ParticleSystemEditor::CreateOfGpu() {
	auto& newParticles = gpuEmitterList_.emplace_back(std::make_unique<GpuParticleEmitter>());
	newParticles->Init(newParticleName_);
	newParticles->SetParticleResourceHandle(gpuParticleRenderer_->GetResourceHandle());
	newParticles->SetFreeListIndexHandle(gpuParticleRenderer_->GetFreeListIndexHandle());
	newParticles->SetFreeListHandle(gpuParticleRenderer_->GetFreeListHandle());
	newParticles->SetMaxParticleResource(gpuParticleRenderer_->GetMaxBufferResource());
	return newParticles.get();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ Particleを追加する
///////////////////////////////////////////////////////////////////////////////////////////////

void ParticleSystemEditor::AddList(const std::string& _name) {
	auto& newParticle = cpuEmitterList_.emplace_back(std::make_unique<AOENGINE::BaseParticles>());
	newParticle->Init(_name);
	std::string textureName = newParticle->GetUseTexture();
	newParticle->SetShareMaterial(
		particleRenderer_->AddParticle(newParticle->GetName(),
									   textureName,
									   newParticle->GetMesh(),
									   newParticle->GetIsAddBlend())
	);

	newParticle->GetShareMaterial()->SetAlbedoTexture(newParticle->GetUseTexture());
	particleUpdater_.Add(_name);
	newParticle->SetParticlesList(particleUpdater_.GetParticles(_name));
	newParticle->SetIsStop(false);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 読み込むためのダイアログを開く
///////////////////////////////////////////////////////////////////////////////////////////////

void ParticleSystemEditor::OpenLoadDialog() {
	// configデータを作成する
	IGFD::FileDialogConfig config;
	config.path = "./Game/Load/Particles/"; // 初期ディレクトリ

	// Dialogを開く
	ImGuiFileDialog::Instance()->OpenDialog(
		"LoadParticlesDialogKey",              // ダイアログ識別キー
		"Load Particles Json File",                 // ウィンドウタイトル
		".json",                           // 設定
		config                           // userDatas（不要ならnullptr）
	);

	// Dialog内の入力処理
	if (ImGuiFileDialog::Instance()->Display("LoadParticlesDialogKey")) {
		if (ImGuiFileDialog::Instance()->IsOk()) {
			std::string filePath = ImGuiFileDialog::Instance()->GetFilePathName();
			std::string fileName = ImGuiFileDialog::Instance()->GetCurrentFileName();
			std::string nameWithoutExtension = std::filesystem::path(fileName).stem().string();

			isLoad_ = false;
			AddList(nameWithoutExtension);
			
		} else {
			// Cancel時の処理
			isLoad_ = false;
		}
		ImGuiFileDialog::Instance()->Close();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集を行う
///////////////////////////////////////////////////////////////////////////////////////////////

void ParticleSystemEditor::InspectorWindow() {
	Update();

	InputText();
	Create();

	// 編集したいParticleの指定を行う
	ImGui::Begin("List");
	static AOENGINE::BaseParticles* cpuParticles = nullptr;
	static GpuParticleEmitter* gpuParticles = nullptr;
	static std::string openNode = "";
	static bool selectCpu = false;
	for (auto& it : cpuEmitterList_) {
		AOENGINE::BaseParticles* ptr = it.get();
		if (ImGui::Selectable(ptr->GetName().c_str(), cpuParticles == ptr)) {
			cpuParticles = it.get();
			openNode = "";  // 他のノードを閉じる
			selectCpu = true;
		}
	}
	for (auto& emitter : gpuEmitterList_) {
		GpuParticleEmitter* ptr = emitter.get();
		if (ImGui::Selectable(ptr->GetName().c_str(), gpuParticles == ptr)) {
			gpuParticles = emitter.get();
			openNode = "";  // 他のノードを閉じる
			selectCpu = false;
		}
	}
	if (ImGui::BeginPopupContextWindow()) {
		if (ImGui::MenuItem("Delete")) {
			if (selectCpu) {
				AOENGINE::BaseParticles* target = cpuParticles;
				bool deleted = false;
				cpuEmitterList_.remove_if([&](const std::unique_ptr<AOENGINE::BaseParticles>& ptr) {
					if (ptr.get() == target) {
						deleted = true;
						return true;
					}
					return false;
										  });
				if (deleted) {
					cpuParticles = nullptr;
				}
			} else {
				GpuParticleEmitter* target = gpuParticles;
				bool deleted = false;
				gpuEmitterList_.remove_if([&](const std::unique_ptr<GpuParticleEmitter>& ptr) {
					if (ptr.get() == target) {
						deleted = true;
						return true;
					}
					return false;
										  });
				if (deleted) {
					gpuParticles = nullptr;
				}
			}
		}
		ImGui::EndPopup();
	}
	ImGui::End();

	// 指定されたParticleの編集を行う
	ImGui::Begin("Setting");
	if (cpuParticles != nullptr) {
		// particle自体を編集する
		cpuParticles->Debug_Gui();

	} else if (gpuParticles != nullptr) {
		gpuParticles->Debug_Gui();
	}

	ImGui::End();
}

void AOENGINE::ParticleSystemEditor::HierarchyWindow() {

}

void AOENGINE::ParticleSystemEditor::ExecutionWindow() {
	Draw();
}

void AOENGINE::ParticleSystemEditor::ClearBuffer() {
	descriptorHeaps_->FreeDSV(depthHandle_.assignIndex_);
	depthStencilResource_.Reset();
}

void AOENGINE::ParticleSystemEditor::ResizeBuffer() {
	depthStencilResource_ = CreateDepthStencilTextureResource(pDevice_, WinApp::sClientWidth, WinApp::sClientHeight);
	// DSVの生成
	D3D12_DEPTH_STENCIL_VIEW_DESC desc{};
	desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;

	depthHandle_ = descriptorHeaps_->AllocateDSV();
	pDevice_->CreateDepthStencilView(depthStencilResource_.Get(), &desc, depthHandle_.handleCPU);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ AOENGINE::RenderTargetをEffectEditer用にする
///////////////////////////////////////////////////////////////////////////////////////////////

void ParticleSystemEditor::SetRenderTarget() {
	// AOENGINE::RenderTargetを指定する
	std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> rtvHandles;
	std::vector<RenderTargetType> types(1, RenderTargetType::EffectSystem_RenderTarget);
	rtvHandles.reserve(types.size());
	for (size_t index = 0; index < types.size(); ++index) {
		rtvHandles.push_back(renderTarget_->GetRenderTargetRTVHandle(EffectSystem_RenderTarget).handleCPU);
	}

	commandList_->OMSetRenderTargets(static_cast<UINT>(rtvHandles.size()), rtvHandles.data(), FALSE, &depthHandle_.handleCPU);
	renderTarget_->SetRenderTarget(commandList_, types, depthHandle_);
	commandList_->ClearDepthStencilView(depthHandle_.handleCPU, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
	float clearColor[] = { 2.0f / 255, 2.0f / 255, 3.0f / 255.0f, 255.0f };
	// AOENGINE::RenderTargetをクリアする
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

	if (ImGui::IsWindowFocused()) {
		EditorWindows::GetInstance()->SetSelectWindow(this);
	}

	// Grid線描画
	AOENGINE::Render::Update();

	if (AOENGINE::EditorWindows::GetInstance()->GetGridDraw()) {
		DrawGrid(camera_->GetViewMatrix(), camera_->GetProjectionMatrix());
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 描画後処理
///////////////////////////////////////////////////////////////////////////////////////////////

void ParticleSystemEditor::PostDraw() {
	AOENGINE::Render::PrimitiveDrawCall();

	// 最後にImGui上でEffectを描画する
	renderTarget_->TransitionResource(commandList_, EffectSystem_RenderTarget, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	ImTextureID textureID2 = reinterpret_cast<ImTextureID>(static_cast<uint64_t>(renderTarget_->GetRenderTargetSRVHandle(RenderTargetType::EffectSystem_RenderTarget).handleGPU.ptr));
	ImGui::SetCursorPos(ImVec2(20, 60)); // 描画位置を設定
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