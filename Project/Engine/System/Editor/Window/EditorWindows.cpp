#include "EditorWindows.h"
#include <algorithm>
#include "Engine/System/Manager/ImGuiManager.h"
#include "Engine/WinApp/WinApp.h"
#include "Engine/Render/Render.h"
#include "Engine/Lib/GameTimer.h"
#include "Engine/Lib/Json/JsonItems.h"
#include "Engine/System/Manager/TextureManager.h"
#include "Engine/System/Editor/Window/Item/ColliderCategorySettingWindow.h"
#include "Engine/System/Editor/Inspector/InspectorRegistration.h"
#include "Engine/System/Scene/SceneSerializer.h"

using namespace AOENGINE;

EditorWindows::~EditorWindows() {}
EditorWindows* AOENGINE::EditorWindows::GetInstance() {
	static EditorWindows instance;
	return &instance;
}

void EditorWindows::Finalize() {
	particleSystemEditor_->Finalize();
	shaderGraphEditor_.reset();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　初期化処理
//////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef _DEVELOPMENT
void EditorWindows::Init(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, AOENGINE::RenderTarget* renderTarget, AOENGINE::DescriptorHeap* descriptorHeaps) {
	RegisterDefaultInspectors();

	packagesWindow_ = std::make_unique<AssetsWindow>();
	packagesWindow_->Init();

	gameObjectWindow_ = std::make_unique<GameObjectWindow>();
	gameObjectWindow_->Init();

	particleSystemEditor_ = std::make_unique<ParticleSystemEditor>();
	particleSystemEditor_->Init(device, commandList, renderTarget, descriptorHeaps);

	manipulateTool_ = std::make_unique<ManipulateTool>();

	shaderGraphEditor_ = std::make_unique<ShaderGraphEditor>();
	shaderGraphEditor_->Init();

	pSelectWindow_ = gameObjectWindow_.get();

	sceneReset_ = false;
	colliderDraw_ = false;
	gridDraw_ = false;
	isSkip_ = false;
	isFullScreen_ = false;
	viewShadowMap_ = false;
	playState_ = EditorPlayState::Edit;
	stepRequested_ = false;
	editSceneSnapshot_ = nlohmann::json();
	editRuntimeHandles_.clear();
	GameTimer::SetTimeScale(0.0f);

	windowItems_.push_back(std::make_unique<ColliderCategorySettingWindow>());
	for (auto& item : windowItems_) {
		item->Init();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　更新処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void EditorWindows::Update() {
	gameObjectWindow_->SetCanvas2d(canvas2d_);
	gameObjectWindow_->SetProcessedSceneFrames(processedSceneFrame_, editorSceneFrame_);
	gameObjectWindow_->SetSceneRenderer(sceneRenderer_);
	
	// sceneを描画する
	if (isFullScreen_) {
		ImGui::Begin("Game");
		processedSceneFrame_->DrawScene();
		ImGui::End();
	} else {
		shaderGraphEditor_->ExecutionWindow();
		particleSystemEditor_->ExecutionWindow();
		gameObjectWindow_->ExecutionWindow();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　開始時処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void EditorWindows::Begin() {

	// -------------------------------------------------
	// ↓ BaseとなるWidowの描画開始
	// -------------------------------------------------
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImVec2(static_cast<float>(WinApp::sClientWidth), static_cast<float>(WinApp::sClientHeight)));
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar |
		ImGuiWindowFlags_NoBringToFrontOnFocus |
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoCollapse;

	if (ImGui::Begin("BaseWindow", nullptr, window_flags)) {
		if (ImGui::BeginMenuBar()) {
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

			if (ImGui::BeginMenu("View")) {
				ImGui::Checkbox("FullScreen", &isFullScreen_);
				ImGui::Checkbox("viewShadowMap", &viewShadowMap_);
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Setting")) {
				for (int i = 0; i < windowItems_.size(); i++) {
					if (ImGui::MenuItem(windowItems_[i]->GetName().c_str(), nullptr, &windowItems_[i]->GetIsActive())) {}
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

	}

	// Debugのためのボタンを表示する
	DebugItemWindow();

	// ドッキングスペーズの確保
	ImGuiID dockspace_id = ImGui::GetID("BaseDockspace");
	ImGui::DockSpace(dockspace_id, ImVec2(0, 0), ImGuiDockNodeFlags_None);

	// 一番上のbegineの分
	ImGui::End();

	gameObjectWindow_->SetCanvas2d(canvas2d_);
	gameObjectWindow_->SetProcessedSceneFrames(processedSceneFrame_, editorSceneFrame_);
	gameObjectWindow_->SetSceneRenderer(sceneRenderer_);

	if (viewShadowMap_) {
		AOENGINE::Render::GetShadowMap()->Debug_Gui(&viewShadowMap_);
	}

	// -------------------------------------------------
	// ↓ 編集項目の表示
	// -------------------------------------------------
	if (!isFullScreen_) {
		manipulateTool_->SelectUseManipulate();

		pSelectWindow_->HierarchyWindow();
		pSelectWindow_->InspectorWindow();

		packagesWindow_->HierarchyWindow();

		if (pSceneManager_ != nullptr) {
			pSceneManager_->Debug_Gui();
		}
	}

	for (auto& item : windowItems_) {
		if (item->GetIsActive()) {
			item->Edit();
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　終了時処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void EditorWindows::End() {
	particleSystemEditor_->End();
}

void EditorWindows::ClearBuffer() {
	particleSystemEditor_->ClearBuffer();
}

void EditorWindows::ResizeBuffer() {
	particleSystemEditor_->ResizeBuffer();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　Gameの描画を行う
//////////////////////////////////////////////////////////////////////////////////////////////////

void EditorWindows::GameWindow() {
	gameObjectWindow_->HierarchyWindow();
	gameObjectWindow_->InspectorWindow();
	ISceneObject* selectedObject = dynamic_cast<ISceneObject*>(gameObjectWindow_->GetSelectObject());
	if (selectedObject && selectedObject->IsActive()) {
		selectedObject->Manipulate(processedSceneFrame_->GetAvailSize(), processedSceneFrame_->GetImagePos());
	}

	manipulateTool_->SelectUseManipulate();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　Debug機能を描画するWindow
//////////////////////////////////////////////////////////////////////////////////////////////////

void EditorWindows::DebugItemWindow() {
	TextureManager* tex = AOENGINE::TextureManager::GetInstance();
	ImVec2 iconSize(16, 16);
	D3D12_GPU_DESCRIPTOR_HANDLE playHandle = tex->GetDxHeapHandles("play.png").handleGPU;
	D3D12_GPU_DESCRIPTOR_HANDLE pauseHandle = tex->GetDxHeapHandles("pause.png").handleGPU;
	D3D12_GPU_DESCRIPTOR_HANDLE skipHandle = tex->GetDxHeapHandles("skip.png").handleGPU;
	D3D12_GPU_DESCRIPTOR_HANDLE colliderHandle = tex->GetDxHeapHandles("collider.png").handleGPU;
	D3D12_GPU_DESCRIPTOR_HANDLE gridHandle = tex->GetDxHeapHandles("grid.png").handleGPU;
	D3D12_GPU_DESCRIPTOR_HANDLE replayHandle = tex->GetDxHeapHandles("replay.png").handleGPU;
	D3D12_GPU_DESCRIPTOR_HANDLE saveHandle = tex->GetDxHeapHandles("save.png").handleGPU;

	ImTextureID saveTex = reinterpret_cast<ImTextureID>(saveHandle.ptr);
	ImTextureID playTex = reinterpret_cast<ImTextureID>(playHandle.ptr);
	ImTextureID pauseTex = reinterpret_cast<ImTextureID>(pauseHandle.ptr);
	ImTextureID skipTex = reinterpret_cast<ImTextureID>(skipHandle.ptr);
	ImTextureID colliderTex = reinterpret_cast<ImTextureID>(colliderHandle.ptr);
	ImTextureID gridTex = reinterpret_cast<ImTextureID>(gridHandle.ptr);
	ImTextureID replayTex = reinterpret_cast<ImTextureID>(replayHandle.ptr);

	if (ImGui::ImageButton("##save", saveTex, iconSize)) {
		if (pSceneManager_ != nullptr && playState_ == EditorPlayState::Edit) {
			pSceneManager_->SaveScene();
		}
	}
	ImGui::SameLine();

	ImVec2 winPos = ImGui::GetWindowPos();
	ImVec2 winSize = ImGui::GetWindowSize();
	// ウィンドウ中央
	float centerX = winPos.x + winSize.x * 0.5f;
	float startX = centerX - iconSize.x * 0.5f * iconSize.x;
	ImGui::SetCursorPosX(startX);
	const bool isInPlayMode = playState_ != EditorPlayState::Edit;
	const ImTextureID playModeIcon = isInPlayMode ? replayTex : playTex;
	if (ImGui::ImageButton("##playMode", playModeIcon, iconSize)) {
		if (isInPlayMode) { ExitPlayMode(); }
		else { EnterPlayMode(); }
	}
	ImGui::SameLine();

	bool pushButton = PushStyleColor(playState_ == EditorPlayState::Paused, Math::Vector4(25, 25, 112, 255.0f));
	ImGui::BeginDisabled(!isInPlayMode);
	if (ImGui::ImageButton("##pausePlayMode", pauseTex, iconSize)) {
		TogglePause();
	}
	ImGui::EndDisabled();
	PopStyleColor(pushButton);
	ImGui::SameLine();

	ImGui::BeginDisabled(playState_ != EditorPlayState::Paused);
	if (ImGui::ImageButton("##stepPlayMode", skipTex, iconSize)) {
		RequestStep();
	}
	ImGui::EndDisabled();
	ImGui::SameLine();

	// -------------------------------------------------
	// ↓ colliderの描画チェック
	// -------------------------------------------------
	pushButton = PushStyleColor(colliderDraw_, Math::Vector4(25, 25, 112, 255.0f));
	if (ImGui::ImageButton("##collider", colliderTex, iconSize)) {
		colliderDraw_ = !colliderDraw_;  // 状態トグル
	}
	PopStyleColor(pushButton);
	ImGui::SameLine();

	// -------------------------------------------------
	// ↓ gridの描画チェック
	// -------------------------------------------------
	pushButton = PushStyleColor(gridDraw_, Math::Vector4(25, 25, 112, 255.0f));
	if (ImGui::ImageButton("##grid", gridTex, iconSize)) {
		gridDraw_ = !gridDraw_;  // 状態トグル
	}
	PopStyleColor(pushButton);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　ButtonColorの変更
//////////////////////////////////////////////////////////////////////////////////////////////////
bool EditorWindows::PushStyleColor(bool _flag, const Math::Vector4& color) {
	bool isChangeColor = false;
	if (_flag) {
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(color.x / 255.0f, color.y / 255.0f, color.z / 255.0f, color.w / 255.0f)); // 背景色
		isChangeColor = true;
	}

	return isChangeColor;
}

void EditorWindows::PopStyleColor(bool _flag) {
	if (_flag) {
		ImGui::PopStyleColor(1);
	}
}

void EditorWindows::SceneReset() {
	gameObjectWindow_->Init();
	sceneReset_ = false;
}

void EditorWindows::EnterPlayMode() {
	if (playState_ != EditorPlayState::Edit || !sceneRenderer_ || !canvas2d_) { return; }
	editSceneSnapshot_ = SceneSerializer::Serialize("PlayModeSnapshot", *sceneRenderer_);
	editRuntimeHandles_ = sceneRenderer_->GetObjectHandles();
	playState_ = EditorPlayState::Playing;
	stepRequested_ = false;
	GameTimer::SetTimeScale(1.0f);
}

void EditorWindows::ExitPlayMode() {
	if (playState_ == EditorPlayState::Edit) { return; }
	GameTimer::SetTimeScale(0.0f);
	stepRequested_ = false;
	if (sceneRenderer_ && canvas2d_ && !editSceneSnapshot_.is_null()) {
		// Play中に新たに生成されたRuntimeOnlyオブジェクトも破棄する。
		for (const ObjectHandle& handle : sceneRenderer_->GetObjectHandles()) {
			const bool existedBeforePlay = std::find(editRuntimeHandles_.begin(), editRuntimeHandles_.end(), handle) != editRuntimeHandles_.end();
			SceneObject* object = sceneRenderer_->FindObject(handle);
			if (!existedBeforePlay && object && object->GetScenePersistence() == ScenePersistence::RuntimeOnly) {
				sceneRenderer_->DestroyObject(handle);
			}
		}
		SceneSerializer::Deserialize(editSceneSnapshot_, *sceneRenderer_, *canvas2d_);
	}
	gameObjectWindow_->Init();
	playState_ = EditorPlayState::Edit;
	editSceneSnapshot_ = nlohmann::json();
	editRuntimeHandles_.clear();
}

void EditorWindows::TogglePause() {
	if (playState_ == EditorPlayState::Playing) {
		playState_ = EditorPlayState::Paused;
		GameTimer::SetTimeScale(0.0f);
	} else if (playState_ == EditorPlayState::Paused) {
		playState_ = EditorPlayState::Playing;
		GameTimer::SetTimeScale(1.0f);
	}
	stepRequested_ = false;
}

void EditorWindows::RequestStep() {
	if (playState_ != EditorPlayState::Paused) { return; }
	stepRequested_ = true;
	GameTimer::SetTimeScale(1.0f);
}

bool EditorWindows::ShouldUpdateGame() const {
	return playState_ == EditorPlayState::Playing || stepRequested_;
}

void EditorWindows::CompleteGameUpdate() {
	if (playState_ == EditorPlayState::Paused && stepRequested_) {
		stepRequested_ = false;
		GameTimer::SetTimeScale(0.0f);
	}
}
#endif 

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　Objectの追加
//////////////////////////////////////////////////////////////////////////////////////////////////

void AOENGINE::EditorWindows::AddPostProcessWindow([[maybe_unused]] AOENGINE::PostProcess* postProcess, [[maybe_unused]] const std::string& label) {
#ifdef _DEVELOPMENT
	GetInstance()->GetObjectWindow()->AddPostProcess(postProcess, label);
#endif
}
