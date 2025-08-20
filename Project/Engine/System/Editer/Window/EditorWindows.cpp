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

	manipulateTool_ = std::make_unique<ManipulateTool>();

	windowUpdate_ = std::bind(&EditorWindows::GameWindow, this);

	sceneReset_ = false;
	openParticleEditor_ = false;
	colliderDraw_ = false;
	gridDraw_ = false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　更新処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void EditorWindows::Update() {
	// 現在選択されているwindowを描画する
	windowUpdate_();

	float fps = 1.0f / GameTimer::DeltaTime();
	ImGui::Text("%f fps", fps);
	ImGui::Text("%f m/s", GameTimer::DeltaTime() * 1000.0f);

	ImGui::End();
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

	// 一番上のbegineの分
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
		processedSceneFrame_->DrawScene();

		if (ManipulateTool::isActive_) {
			if (sceneRenderer_ != nullptr) {
				sceneRenderer_->EditObject(processedSceneFrame_->GetAvailSize(), processedSceneFrame_->GetImagePos());
			}
		}

		manipulateTool_->SelectUseManipulate();
	}
	gameObjectWindow_->Edit();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　Effectの編集を行う描画
//////////////////////////////////////////////////////////////////////////////////////////////////

void EditorWindows::ParticleEditorWindow() {
	openParticleEditor_ = false;
	if (ImGui::Begin("ParticleSystemEditor", nullptr)) {
		particleSystemEditor_->Update();
		particleSystemEditor_->Draw();
		openParticleEditor_ = true;
	}
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

		static bool isPlaying = true;  // トグル状態を保持
		ImTextureID icon = isPlaying ? pauseTex : playTex;
		if (ImGui::ImageButton("##toggle", icon, iconSize)) {
			isPlaying = !isPlaying;
			GameTimer::SetTimeScale(isPlaying ? 1.0f : 0.0f);  // 再生・停止
		}
		ImGui::SameLine();

		bool pushButton = false;
		// -------------------------------------------------
		// ↓ skipの描画チェック
		// -------------------------------------------------

		if (isPlaying) {
			pushButton = PushStyleColor(true, Vector4(34.0f, 34.0f, 32.0f, 255.0f));
		} else {
			pushButton = PushStyleColor(false, Vector4(25, 25, 112, 255.0f));
		}
		if (ImGui::ImageButton("##skip", skipTex, iconSize)) {
			GameTimer::SetTimeScale(1.0f);  // 再生・停止
		}
		PopStyleColor(pushButton);
		ImGui::SameLine();

		// -------------------------------------------------
		// ↓ Replayの描画チェック
		// -------------------------------------------------
		pushButton = PushStyleColor(sceneReset_, Vector4(25, 25, 112, 255.0f));
		if (ImGui::ImageButton("##replay", replayTex, iconSize)) {
			sceneReset_ = !sceneReset_;  // 状態トグル
		}
		PopStyleColor(pushButton);
		ImGui::SameLine();

		// -------------------------------------------------
		// ↓ colliderの描画チェック
		// -------------------------------------------------
		pushButton = PushStyleColor(colliderDraw_, Vector4(25, 25, 112, 255.0f));
		if (ImGui::ImageButton("##collider", colliderTex, iconSize)) {
			colliderDraw_ = !colliderDraw_;  // 状態トグル
		}
		PopStyleColor(pushButton);
		ImGui::SameLine();

		// -------------------------------------------------
		// ↓ gridの描画チェック
		// -------------------------------------------------
		pushButton = PushStyleColor(gridDraw_, Vector4(25, 25, 112, 255.0f));
		if (ImGui::ImageButton("##grid", gridTex, iconSize)) {
			gridDraw_ = !gridDraw_;  // 状態トグル
		}
		PopStyleColor(pushButton);
	}
	ImGui::End();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　ButtonColorの変更
//////////////////////////////////////////////////////////////////////////////////////////////////
bool EditorWindows::PushStyleColor(bool _flag, const Vector4& color) {
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

void EditorWindows::AddObjectWindow(AttributeGui* attribute, const std::string& label) {
#ifdef _DEBUG
	attribute->SetName(label);
	GetInstance()->GetObjectWindow()->AddAttributeGui(attribute, label);
#endif 
}