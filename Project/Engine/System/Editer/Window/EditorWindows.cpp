#include "EditorWindows.h"
#include "Engine/System/Manager/ImGuiManager.h"
#include "Engine/WinApp/WinApp.h"
#include "Engine/Render.h"
#include "Engine/Lib/GameTimer.h"
#include "Engine/Lib/Json/JsonItems.h"
#include "Engine/System/Manager/TextureManager.h"

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

#ifdef _DEBUG
void EditorWindows::Init(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, AOENGINE::RenderTarget* renderTarget, AOENGINE::DescriptorHeap* descriptorHeaps) {
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
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　更新処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void EditorWindows::Update() {
	gameObjectWindow_->SetCanvas2d(canvas2d_);
	gameObjectWindow_->SetProcessedSceneFrame(processedSceneFrame_);
	gameObjectWindow_->SetSceneRenderer(sceneRenderer_);

	ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar
		| ImGuiWindowFlags_AlwaysAutoResize
		| ImGuiWindowFlags_NoScrollbar
		| ImGuiWindowFlags_NoCollapse
		| ImGuiWindowFlags_NoDocking
		| ImGuiWindowFlags_NoBackground;
	if (ImGui::Begin("AOENGINE::GameTimer", nullptr, flags)) {
		float fps = 1.0f / AOENGINE::GameTimer::DeltaTime();
		ImGui::Text("%f fps", fps);
		ImGui::Text("%f m/s", AOENGINE::GameTimer::DeltaTime() * 1000.0f);
	}
	ImGui::End();

	// sceneを描画する
	if (isFullScreen_) {
		processedSceneFrame_->DrawScene();
	} else {
		shaderGraphEditor_->ExecutionWindow();
		particleSystemEditor_->ExecutionWindow();
		gameObjectWindow_->ExecutionWindow();

		manipulateTool_->SelectUseManipulate();

		pSelectWindow_->HierarchyWindow();
		pSelectWindow_->InspectorWindow();
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
			}
			if (ImGui::BeginMenu("View")) {
				ImGui::Checkbox("FullScreen", &isFullScreen_);
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
	if (ManipulateTool::isActive_) {
		if (ManipulateTool::is3dManipulate_) {
			if (sceneRenderer_ != nullptr) {
				sceneRenderer_->EditObject(processedSceneFrame_->GetAvailSize(), processedSceneFrame_->GetImagePos());
			}
		} else {
			if (canvas2d_ != nullptr) {
				canvas2d_->EditObject(processedSceneFrame_->GetAvailSize(), processedSceneFrame_->GetImagePos());
			}
		}
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

	ImTextureID playTex = reinterpret_cast<ImTextureID>(playHandle.ptr);
	ImTextureID pauseTex = reinterpret_cast<ImTextureID>(pauseHandle.ptr);
	ImTextureID skipTex = reinterpret_cast<ImTextureID>(skipHandle.ptr);
	ImTextureID colliderTex = reinterpret_cast<ImTextureID>(colliderHandle.ptr);
	ImTextureID gridTex = reinterpret_cast<ImTextureID>(gridHandle.ptr);
	ImTextureID replayTex = reinterpret_cast<ImTextureID>(replayHandle.ptr);

	ImVec2 winPos = ImGui::GetWindowPos();
	ImVec2 winSize = ImGui::GetWindowSize();
	// ウィンドウ中央
	float centerX = winPos.x + winSize.x * 0.5f;
	float startX = centerX - iconSize.x * 0.5f * iconSize.x;
	ImGui::SetCursorPosX(startX);
	static bool isPlaying = true;  // トグル状態を保持
	ImTextureID icon = isPlaying ? pauseTex : playTex;
	if (ImGui::ImageButton("##toggle", icon, iconSize)) {
		isPlaying = !isPlaying;
		isSkip_ = false;
		AOENGINE::GameTimer::SetTimeScale(isPlaying ? 1.0f : 0.0f);  // 再生・停止
	}
	ImGui::SameLine();

	bool pushButton = false;
	// -------------------------------------------------
	// ↓ skipの描画チェック
	// -------------------------------------------------

	if (isPlaying) {
		pushButton = PushStyleColor(true, Math::Vector4(34.0f, 34.0f, 32.0f, 255.0f));
	} else {
		pushButton = PushStyleColor(false, Math::Vector4(25, 25, 112, 255.0f));
	}
	if (isSkip_) {
		AOENGINE::GameTimer::SetTimeScale(0.0f);  // 再生・停止
		isSkip_ = false;
	}
	if (ImGui::ImageButton("##skip", skipTex, iconSize)) {
		AOENGINE::GameTimer::SetTimeScale(1.0f);  // 再生・停止
		isSkip_ = true;
	}
	PopStyleColor(pushButton);
	ImGui::SameLine();

	// -------------------------------------------------
	// ↓ Replayの描画チェック
	// -------------------------------------------------
	pushButton = PushStyleColor(sceneReset_, Math::Vector4(25, 25, 112, 255.0f));
	if (ImGui::ImageButton("##replay", replayTex, iconSize)) {
		sceneReset_ = !sceneReset_;  // 状態トグル
	}
	PopStyleColor(pushButton);
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
#endif 

void EditorWindows::Reset() {
	gameObjectWindow_->Init();
	sceneReset_ = false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　Objectの追加
//////////////////////////////////////////////////////////////////////////////////////////////////

void AOENGINE::EditorWindows::AddObjectWindow([[maybe_unused]] AOENGINE::AttributeGui* attribute, [[maybe_unused]] const std::string& label) {
#ifdef _DEBUG
	attribute->SetName(label);
	GetInstance()->GetObjectWindow()->AddAttributeGui(attribute, label);
#endif
}