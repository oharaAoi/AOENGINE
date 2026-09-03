#include "SceneManager.h"
#include <filesystem>
#include <optional>
#include "Engine/Core/Engine.h"
#include "Engine/Render/Render.h"
#include "Engine/Render/ShadowMap.h"
#include "Engine/System/Manager/ParticleManager.h"
#include "Engine/System/Manager/GpuParticleManager.h"
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
	scene_.reset();
	// 全シーンで共有するRendererは、SceneManager自体の終了時に一度だけ終了する。
	AOENGINE::SceneRenderer::GetInstance()->Finalize();
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
	if (scene_->GetNextSceneType()) {
		SceneType type = scene_->GetNextSceneType().value();
		// SetChange()でscene_自体が置き換わる前に、旧シーン側の要求を消費する。
		scene_->SetNextSceneType(std::nullopt);
		SetChange(type);
		// RendererとEditorの参照が切り替わったフレームでは更新を続けない。
		// 新しいシーンのEditorUpdate/Updateは次フレームから開始する。
		return false;
	}
	
	if (reset_ || AOENGINE::EditorWindows::GetInstance()->GetSceneReset()) {
#ifdef _DEVELOPMENT
		AOENGINE::EditorWindows::GetInstance()->SceneReset();
#endif
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
		scene_->SetNextSceneType(changeScene_);
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
	nextScene_ = sceneFactory_->CreateScene(type);
	scene_ = std::move(nextScene_);
	auto name = magic_enum::enum_name(type);
	scene_->SetSceneName(name.data());

#ifdef _DEVELOPMENT
	AOENGINE::EditorWindows::GetInstance()->SceneReset();
#endif // _DEVELOPMENT

	ResetManager();

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

	gpuManager->Finalize();
	cpuManager->Finalize();
	Engine::GetCanvas2d()->Init();

	// PostProcessは新しいSceneRenderer::Init()後のSetSceneRenderer()から再登録する。
	// ここで登録すると、破棄予定の旧SceneWorldへ一度生成されてしまう。
}
