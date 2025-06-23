#include "EditorWindows.h"
#include "Engine/Lib/Json/JsonItems.h"
#include "Engine/System/Manager/ImGuiManager.h"

EditorWindows::~EditorWindows() {}
EditorWindows* EditorWindows::GetInstance() {
	static EditorWindows instance;
	return &instance;
}

void EditorWindows::Finalize() {
	particleSystemEditor_->Finalize();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　初期化処理
//////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
void EditorWindows::Init(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, RenderTarget* renderTarget, DescriptorHeap* descriptorHeaps) {
	gameObjectWindow_ = std::make_unique<GameObjectWindow>();
	gameObjectWindow_->Init();

	particleSystemEditor_ = std::make_unique<ParticleSystemEditor>();
	particleSystemEditor_->Init(device, commandList, renderTarget, descriptorHeaps);

	windowUpdate_ = std::bind(&EditorWindows::GameWindow, this);

	openParticleEditor_ = false;
	colliderDraw_ = false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　更新処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void EditorWindows::Update() {
	// 現在選択されているwindowを描画する
	windowUpdate_();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　開始時処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void EditorWindows::Begin() {

	// -------------------------------------------------
	// ↓ BaseとなるWidowの描画開始
	// -------------------------------------------------
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImVec2(kWindowWidth_, kWindowHeight_));
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar |
		ImGuiWindowFlags_NoBringToFrontOnFocus |
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoCollapse;

	if (ImGui::Begin("BaseWindow", nullptr, window_flags)) {
		if (ImGui::BeginMenuBar()) {
			// -------------------------------------------------
			// ↓ fileに関するMenu
			// -------------------------------------------------
			if (ImGui::BeginMenu("File")) {
				// json
				if (ImGui::BeginMenu("JsonItems")) {
					if (ImGui::Button("HotReload")) {
						JsonItems::GetInstance()->LoadAllFile();
						MessageBoxA(nullptr, "HotReload", "JsonItems", 0);
					}
					ImGui::EndMenu();
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

	}
	ImGuiID dockspace_id = ImGui::GetID("BaseDockspace");
	ImGui::DockSpace(dockspace_id, ImVec2(0, 0), ImGuiDockNodeFlags_None);

	DebugItemWindow();

	// -------------------------------------------------
	// ↓ BaseとなるWidowの描画開始
	// -------------------------------------------------

	if (ImGui::Begin("ParticleSystemEditor", nullptr)) {
		if (ImGui::IsWindowFocused()) {
			windowUpdate_ = std::bind(&EditorWindows::ParticleEditorWindow, this);
			openParticleEditor_ = true;
		}
	}
	ImGui::End();

	if (ImGui::Begin("Game Window", nullptr)) {
		if (ImGui::IsWindowFocused()) {
			windowUpdate_ = std::bind(&EditorWindows::GameWindow, this);
			openParticleEditor_ = false;
		}
	}
	ImGui::End();

	ImGui::End();

}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　終了時処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void EditorWindows::End() {
	if (openParticleEditor_) {
		particleSystemEditor_->End();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　Gameの描画を行う
//////////////////////////////////////////////////////////////////////////////////////////////////

void EditorWindows::GameWindow() {
	if (ImGui::Begin("Game Window", nullptr)) {
		processedSceneFrame_->DrawGui();
	}
	ImGui::End();

	gameObjectWindow_->Edit();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　Effectの編集を行う描画
//////////////////////////////////////////////////////////////////////////////////////////////////

void EditorWindows::ParticleEditorWindow() {
	if (ImGui::Begin("ParticleSystemEditor", nullptr)) {
		particleSystemEditor_->Update();
		particleSystemEditor_->Draw();
	}
	ImGui::End();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　Debug機能を描画するWindow
//////////////////////////////////////////////////////////////////////////////////////////////////

void EditorWindows::DebugItemWindow() {
	ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar
		| ImGuiWindowFlags_AlwaysAutoResize
		| ImGuiWindowFlags_NoScrollbar
		| ImGuiWindowFlags_NoCollapse
		| ImGuiWindowFlags_NoBackground
		| ImGuiWindowFlags_NoMove
		| ImGuiWindowFlags_NoDocking;

	if (ImGui::Begin("DebugItems", nullptr, flags)) {
		TextureManager* tex = TextureManager::GetInstance();
		ImVec2 iconSize(16, 16);
		D3D12_GPU_DESCRIPTOR_HANDLE playHandle = tex->GetDxHeapHandles("play.png").handleGPU;
		D3D12_GPU_DESCRIPTOR_HANDLE pauseHandle = tex->GetDxHeapHandles("pause.png").handleGPU;

		ImTextureID playTex = reinterpret_cast<ImTextureID>(playHandle.ptr);
		ImTextureID pauseTex = reinterpret_cast<ImTextureID>(pauseHandle.ptr);

		static bool isPlaying = true;  // トグル状態を保持
		ImTextureID icon = isPlaying ? pauseTex : playTex;
		if (ImGui::ImageButton("##toggle", icon, iconSize)) {
			isPlaying = !isPlaying;
		}
		GameTimer::SetTimeScale(isPlaying ? 1.0f : 0.0f);  // 再生・停止

		ImGui::SameLine();
		// colliderの描画チェック
		ImGui::Checkbox("colliderDraw", &colliderDraw_);
	}
	ImGui::End();
}
#endif 

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　Objectの追加
//////////////////////////////////////////////////////////////////////////////////////////////////

void EditorWindows::AddObjectWindow(AttributeGui* attribute, const std::string& label) {
#ifdef _DEBUG
	attribute->SetName(label);
	GetInstance()->GetObjectWindow()->AddAttributeGui(attribute, label);
#endif 
}