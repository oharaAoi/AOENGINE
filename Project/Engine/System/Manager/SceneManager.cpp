#include "SceneManager.h"
#include <optional>
#include "Engine.h"
#include "Engine/Render.h"
#include "Engine/Render/ShadowMap.h"
#include "Engine/System/Manager/ParticleManager.h"
#include "Engine/System/Manager/GpuParticleManager.h"
#include "Engine/System/Manager/TextureManager.h"
#include "Engine/System/Editor/Window/EditorWindows.h"
#include "Engine/Module/Components/Light/LightGroup.h"
#include "Engine/Utilities/ImGuiHelperFunc.h"

using namespace AOENGINE;

SceneManager::SceneManager() {}
SceneManager::~SceneManager() {}

void SceneManager::Finalize() {
	scene_.reset();
	systemManager_->Finalize();
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

	AOENGINE::EditorWindows::GetInstance()->SetSceneManager(this);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　更新処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void SceneManager::Update() {
	if (scene_->GetNextSceneType()) {
		SetChange(scene_->GetNextSceneType().value());
		scene_->SetNextSceneType(std::nullopt);
	}
	
	if (reset_ || AOENGINE::EditorWindows::GetInstance()->GetSceneReset()) {
		ResetManager();
		AOENGINE::EditorWindows::GetInstance()->SceneReset();

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

#ifdef _DEBUG
	AOENGINE::EditorWindows::GetInstance()->SceneReset();
#endif // _DEBUG

	ResetManager();

	systemManager_->Init();
	scene_->Init();

	reset_ = false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓ Sceneの情報を保存する
//////////////////////////////////////////////////////////////////////////////////////////////////

void AOENGINE::SceneManager::SaveScene() {
	scene_->SaveScene();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓ Managerのリセット
//////////////////////////////////////////////////////////////////////////////////////////////////

void AOENGINE::SceneManager::ResetManager() {
	AOENGINE::PostProcess* postProcess = Engine::GetPostProcess();
	AOENGINE::EditorWindows::AddObjectWindow(postProcess, "PostProcess");

	AOENGINE::LightGroup* lightGroup = AOENGINE::Render::GetLightGroup();
	lightGroup->GetDirectionalLight()->Reset();
	AOENGINE::EditorWindows::AddObjectWindow(lightGroup, "LightGroup");

	AOENGINE::ShadowMap* shadowMap = AOENGINE::Render::GetShadowMap();
	AOENGINE::EditorWindows::AddObjectWindow(shadowMap, "ShadowMap");

	AOENGINE::ParticleManager* cpuManager = AOENGINE::ParticleManager::GetInstance();
	AOENGINE::GpuParticleManager* gpuManager = AOENGINE::GpuParticleManager::GetInstance();

	gpuManager->Finalize();
	cpuManager->Finalize();
}
