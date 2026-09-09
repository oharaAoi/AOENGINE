#include "SceneManager.h"
#include <filesystem>
#include <optional>
#include "Engine/Core/Engine.h"
#include "Engine/Render/Render.h"
#include "Engine/Render/ShadowMap.h"
#include "Engine/System/Manager/ParticleManager.h"
#include "Engine/System/Manager/GpuParticleManager.h"
#include "Engine/System/Manager/ParticleEffectManager.h"
#include "Engine/System/Manager/TextureManager.h"
#include "Engine/System/Editor/Window/EditorWindows.h"
#include "Engine/System/Scene/SceneManagerPropertySerializer.h"
#include "Engine/System/Scene/SceneSerializer.h"
#include "Engine/Lib/Json/JsonItems.h"
#include "Engine/Module/Components/Light/LightGroup.h"
#include "Engine/Utilities/ImGuiHelperFunc.h"
#include "Engine/Utilities/Logger.h"
#include "Engine/Lib/GameTimer.h"
#include <magic_enum/magic_enum.hpp>

using namespace AOENGINE;

SceneManager::SceneManager() {}
SceneManager::~SceneManager() {}

void SceneManager::Finalize() {
	// 全シーンで共有するRendererは、SceneManager自体の終了時に一度だけ終了する。
	AOENGINE::SceneRenderer::GetInstance()->Finalize();
	scene_.reset();
	systemManager_->Finalize();
	AOENGINE::SceneManagerPropertySerializer::Save(static_cast<int>(nowScene_));
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　初期化処理
//////////////////////////////////////////////////////////////////////////////////////////////////
void SceneManager::Init() {
	// ReleaseビルドにはEditorのPlay状態がないため、起動直後からゲーム時間を進める。
#ifndef _DEVELOPMENT
	GameTimer::SetTimeScale(1.0f);
#endif

	// gameに必要なResourceの読み込み
	systemManager_ = std::make_unique<SystemManager>();
	systemManager_->Init();

	sceneFactory_ = std::make_unique<SceneFactory>();
	sceneTransition_ = std::make_unique<SceneTransition>();
	transitionState_ = TransitionState::Idle;
	pendingSceneType_.reset();
	editorSceneChangeRequest_.reset();
	reset_ = false;

//#ifdef _DEVELOPMENT
//	int sceneType = 0;
//	AOENGINE::SceneManagerPropertySerializer::Load(sceneType);
//	changeScene_ = static_cast<SceneType>(sceneType);
//#endif // _DEVELOPMENT

	AOENGINE::EditorWindows::GetInstance()->SetSceneManager(this);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　更新処理
//////////////////////////////////////////////////////////////////////////////////////////////////

bool SceneManager::Update() {
#ifdef _DEVELOPMENT
	if (editorSceneChangeRequest_) {
		const SceneType type = *editorSceneChangeRequest_;
		editorSceneChangeRequest_.reset();
		pendingSceneType_.reset();
		transitionState_ = TransitionState::Idle;
		if (sceneTransition_) { sceneTransition_->Release(); }
		SetChange(type);
		return false;
	}
#endif
	if (transitionState_ == TransitionState::Idle && scene_->GetNextSceneType()) {
		SceneType type = scene_->GetNextSceneType().value();
		scene_->SetNextSceneType(std::nullopt);
		BeginSceneTransition(type);
	}
	if (UpdateSceneTransition()) {
		return false;
	}
	
	if (reset_ || AOENGINE::EditorWindows::GetInstance()->GetSceneReset()) {
#ifdef _DEVELOPMENT
		AOENGINE::EditorWindows::GetInstance()->SceneReset();
#endif
		// SceneWorldとParticleManagerを空にする前に、ゲーム側が持っているObjectを先に片付ける。
		// 順番が逆だと、既に実体が消えた後にGameScene側の破棄が走ることになり、
		// Sceneの切り替え(SetChange)側の順番とも食い違う。
		scene_->Finalize();
		ResetManager();
		systemManager_->Init();
		scene_->Initialize();
		LoadScene();

		reset_ = false;
	}

#ifdef _DEVELOPMENT
	if (!AOENGINE::EditorWindows::GetInstance()->ShouldUpdateGame()) {
		scene_->EditorUpdateProcess();
		return false;
	}
#endif

	scene_->UpdateProcess();

	systemManager_->Update();

#ifdef _DEVELOPMENT
	AOENGINE::EditorWindows::GetInstance()->CompleteGameUpdate();
#endif

	return scene_->GetEndRequest();
}

void SceneManager::BeginSceneTransition(SceneType type) {
	if (transitionState_ != TransitionState::Idle) { return; }
	pendingSceneType_ = type;
	if (!sceneTransition_->IsReady()) { sceneTransition_->Init(); }
	if (!sceneTransition_->IsReady()) {
		// Overlayを作れない場合もシーン進行を止めない。
		SetChange(type);
		pendingSceneType_.reset();
		return;
	}
	sceneTransition_->FadeIn();
	transitionState_ = TransitionState::Covering;
}

bool SceneManager::UpdateSceneTransition() {
	if (transitionState_ == TransitionState::Idle) { return false; }
	sceneTransition_->Update();
	if (!sceneTransition_->IsFinish()) { return false; }

	if (transitionState_ == TransitionState::Covering) {
		if (!pendingSceneType_) {
			transitionState_ = TransitionState::Idle;
			return false;
		}
		const SceneType nextScene = *pendingSceneType_;
		pendingSceneType_.reset();
		sceneTransition_->Release();
		SetChange(nextScene);
		sceneTransition_->Init();
		if (sceneTransition_->IsReady()) {
			sceneTransition_->SetCovered();
			sceneTransition_->FadeOut();
			transitionState_ = TransitionState::Revealing;
		} else {
			transitionState_ = TransitionState::Idle;
		}
		return true;
	}

	sceneTransition_->SetVisible();
	transitionState_ = TransitionState::Idle;
	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　描画処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void SceneManager::Draw() {
	if (!scene_) {
		return;
	}
	scene_->Draw();
}

void SceneManager::OnPlayStart() {
	if (scene_) {
		scene_->OnPlayStart();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　編集処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void SceneManager::Debug_Gui() {
	// sceneのDebug
	ImGui::Begin("Scene");
	static bool isChange = false;
	TextureManager* tex = AOENGINE::TextureManager::GetInstance();
	D3D12_GPU_DESCRIPTOR_HANDLE handle = tex->GetDxHeapHandles("scene.png").handleGPU;
	ImTextureID texID = reinterpret_cast<ImTextureID>(handle.ptr);
	for (uint32_t index = 0; index < (uint32_t)SceneType::kMax; ++index) {
		SceneType type = static_cast<SceneType>(index);
		auto name = magic_enum::enum_name(type);
		if (DrawImageButtonWithLabel(texID, name.data(), ImVec2(32.f, 32.f))) {
			isChange = true;
			changeScene_ = static_cast<SceneType>(index);
		}
	}

	if (isChange) {
		// Editorからの選択はフレーム境界で即時切替し、画面遷移を挟まない。
		editorSceneChangeRequest_ = changeScene_;
		isChange = false;
	}

	if (ImGui::Button("Save Current Scene")) {
		SaveScene();
	}
	ImGui::SameLine();
	if (ImGui::Button("Load Current Scene")) {
		LoadScene();
	}

	ImGui::End();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　シーンの切り替え
//////////////////////////////////////////////////////////////////////////////////////////////////

void SceneManager::SetChange(const SceneType& type) {
	assert(sceneFactory_);
	assert(nextScene_ == nullptr);
#ifdef _DEVELOPMENT
	// 旧SceneWorldのHandleを使う選択・削除・複製・Drag&Drop要求を先に破棄する。
	if (AOENGINE::EditorWindows::GetInstance()->GetObjectWindow()) {
		AOENGINE::EditorWindows::GetInstance()->GetObjectWindow()->ResetInteractionState();
	}
#endif
	if (scene_ != nullptr) {
		scene_->Finalize();
	}

	ResetManager();

	nextScene_ = sceneFactory_->CreateScene(type);
	scene_ = std::move(nextScene_);
	auto name = magic_enum::enum_name(type);
	scene_->SetSceneName(name.data());

#ifdef _DEVELOPMENT
	AOENGINE::EditorWindows::GetInstance()->SceneReset();
#endif // _DEVELOPMENT

	

	systemManager_->Init();
	scene_->Initialize();
	LoadScene();

	// Releaseはシーン生成直後からPlay状態として開始する。
	// DebugでもPlay中にシーンを切り替えた場合は、新しいシーンへ開始を通知する。
#ifdef _DEVELOPMENT
	if (AOENGINE::EditorWindows::GetInstance()->IsPlaying()) {
		scene_->OnPlayStart();
	}
#else
	scene_->OnPlayStart();
#endif

	reset_ = false;
	nowScene_ = type;
	if (sceneTransition_ && transitionState_ == TransitionState::Idle) {
		// 起動時やEditorからの即時切替でも、次回の遷移用Overlayを用意する。
		sceneTransition_->Release();
		sceneTransition_->Init();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓ Sceneの情報を保存する
//////////////////////////////////////////////////////////////////////////////////////////////////

void AOENGINE::SceneManager::SaveScene() {
#ifdef _DEVELOPMENT
	// Play中のRuntime変更を編集用シーンへ書き込まない。
	if (AOENGINE::EditorWindows::GetInstance()->IsPlaying()) {
		return;
	}
#endif
	const std::string folderPath = AOENGINE::JsonItems::GetDirectoryPath() + scene_->GetSceneName() + "/";
	const std::string& sceneName = scene_->GetSceneName();
	if (!AOENGINE::SceneSerializer::Save(folderPath, sceneName, *AOENGINE::SceneRenderer::GetInstance(),
		*Engine::GetPostProcess())) {
		AOENGINE::Logger::Log("[Scene][Save] Failed: " + (std::filesystem::path(folderPath) / (sceneName + ".json")).string() + "\n");
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓ Sceneの情報を読み込む
//////////////////////////////////////////////////////////////////////////////////////////////////

bool AOENGINE::SceneManager::LoadScene() {
	const std::string& sceneName = scene_->GetSceneName();
	const std::string folderPath = AOENGINE::JsonItems::GetDirectoryPath() + sceneName + "/";
	const std::filesystem::path scenePath = std::filesystem::path(folderPath) / (sceneName + ".json");
	std::error_code error;
	if (!std::filesystem::exists(scenePath, error) || error) {
		AOENGINE::Logger::Log("[Scene][Load] File not found: " + scenePath.string() + "\n");
		return false;
	}

	const bool loaded = AOENGINE::SceneSerializer::Load(
		folderPath, sceneName, *AOENGINE::SceneRenderer::GetInstance(), *Engine::GetCanvas2d(),
		*Engine::GetPostProcess());
	if (!loaded) {
		AOENGINE::Logger::Log("[Scene][Load] Failed to deserialize: " + scenePath.string() + "\n");
	}
	return loaded;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓ Managerのリセット
//////////////////////////////////////////////////////////////////////////////////////////////////

void AOENGINE::SceneManager::ResetManager() {
	AOENGINE::ParticleManager* cpuManager = AOENGINE::ParticleManager::GetInstance();
	AOENGINE::GpuParticleManager* gpuManager = AOENGINE::GpuParticleManager::GetInstance();

	// Canvas/Particleが所有する外部Objectを破棄する前に、
	// SceneWorld内の生ポインタと親子参照をすべて解除する。
	AOENGINE::SceneRenderer::GetInstance()->ClearSceneObjects();

	// Effectが参照するCPU/GPU Emitterを、各Managerの破棄より先に解放する。
	AOENGINE::ParticleEffectManager::GetInstance()->Finalize();
	gpuManager->Finalize();
	cpuManager->Finalize();
	Engine::GetCanvas2d()->Init();

	// PostProcessは新しいSceneRenderer::Init()後のSetSceneRenderer()から再登録する。
	// ここで登録すると、破棄予定の旧SceneWorldへ一度生成されてしまう。
}
