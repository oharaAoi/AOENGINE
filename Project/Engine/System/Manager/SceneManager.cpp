#include "SceneManager.h"
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

using namespace AOENGINE;

SceneManager::SceneManager() {}
SceneManager::~SceneManager() {}

void SceneManager::Finalize() {
	scene_.reset();
	systemManager_->Finalize();
	AOENGINE::SceneManagerPropertySerializer::Save(static_cast<int>(nowScene_));
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　初期化処理
//////////////////////////////////////////////////////////////////////////////////////////////////
void SceneManager::Init() {
	// gameに必要なResourceの読み込み
	systemManager_ = std::make_unique<SystemManager>();
	systemManager_->Init();

	sceneFactory_ = std::make_unique<SceneFactory>();
	reset_ = false;

	changeScene_ = SceneType::Test;
//#ifdef _DEVELOPMENT
//	int sceneType = 0;
//	AOENGINE::SceneManagerPropertySerializer::Load(sceneType);
//	changeScene_ = static_cast<SceneType>(sceneType);
//#endif // _DEVELOPMENT

	SetChange(changeScene_);

	AOENGINE::EditorWindows::GetInstance()->SetSceneManager(this);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　更新処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void SceneManager::Update() {
	if (scene_->GetNextSceneType()) {
		SceneType type = scene_->GetNextSceneType().value();
		SetChange(type);
		scene_->SetNextSceneType(std::nullopt);
	}
	
	if (reset_ || AOENGINE::EditorWindows::GetInstance()->GetSceneReset()) {
#ifdef _DEVELOPMENT
		AOENGINE::EditorWindows::GetInstance()->SceneReset();
#endif
		ResetManager();
		systemManager_->Init();
		scene_->Init();

		reset_ = false;
	}

	scene_->UpdateProcess();

	systemManager_->Update();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　描画処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void SceneManager::Draw() {
	scene_->Draw();
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
		if (DrawImageButtonWithLabel(texID, kSceneTypeNames[index], ImVec2(32.f, 32.f))) {
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
	if (scene_ != nullptr) {
		scene_->Finalize();
	}
	nextScene_ = sceneFactory_->CreateScene(type);
	scene_ = std::move(nextScene_);
	scene_->SetSceneName(kSceneTypeNames[static_cast<size_t>(type)]);

#ifdef _DEVELOPMENT
	AOENGINE::EditorWindows::GetInstance()->SceneReset();
#endif // _DEVELOPMENT

	ResetManager();

	systemManager_->Init();
	scene_->Init();

	reset_ = false;
	nowScene_ = type;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓ Sceneの情報を保存する
//////////////////////////////////////////////////////////////////////////////////////////////////

void AOENGINE::SceneManager::SaveScene() {
	const std::string folderPath = AOENGINE::JsonItems::GetDirectoryPath() + scene_->GetSceneName() + "/";
	AOENGINE::SceneSerializer::Save(folderPath, "scene", *AOENGINE::SceneRenderer::GetInstance());
	scene_->SaveSceneEffect();
}

bool AOENGINE::SceneManager::LoadScene() {
	const std::string folderPath = AOENGINE::JsonItems::GetDirectoryPath() + scene_->GetSceneName() + "/";
	const bool loaded = AOENGINE::SceneSerializer::Load(
		folderPath, "scene", *AOENGINE::SceneRenderer::GetInstance(), *Engine::GetCanvas2d());
	if (loaded) {
		scene_->LoadSceneEffect();
	}
	return loaded;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓ Managerのリセット
//////////////////////////////////////////////////////////////////////////////////////////////////

void AOENGINE::SceneManager::ResetManager() {
	Engine::GetCanvas2d()->Init();
	Engine::RegisterEditorWindowSystem();

	AOENGINE::ParticleManager* cpuManager = AOENGINE::ParticleManager::GetInstance();
	AOENGINE::GpuParticleManager* gpuManager = AOENGINE::GpuParticleManager::GetInstance();

	gpuManager->Finalize();
	cpuManager->Finalize();
}
