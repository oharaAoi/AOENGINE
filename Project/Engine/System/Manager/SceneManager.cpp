#include "SceneManager.h"
#include <optional>
#include "Engine.h"
#include "Engine/System/Manager/ParticleManager.h"
#include "Engine/System/Manager/GpuParticleManager.h"

SceneManager::SceneManager() {}
SceneManager::~SceneManager() {}

void SceneManager::Finalize() {
	scene_.reset();
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
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　更新処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void SceneManager::Update() {
	if (scene_->GetNextSceneType()) {
		SetChange(scene_->GetNextSceneType().value());
		scene_->SetNextSceneType(std::nullopt);
	}

	if (reset_) {
		reset_ = false;

		EditorWindows::GetInstance()->Reset();

		AOENGINE::PostProcess* postProcess = Engine::GetPostProcess();
		EditorWindows::AddObjectWindow(postProcess, "PostProcess");

		AOENGINE::LightGroup* lightGroup = AOENGINE::Render::GetLightGroup();
		lightGroup->GetDirectionalLight()->Reset();
		EditorWindows::AddObjectWindow(lightGroup, "LightGroup");

		AOENGINE::ShadowMap* shadowMap = AOENGINE::Render::GetShadowMap();
		EditorWindows::AddObjectWindow(shadowMap, "ShadowMap");

		auto& window = EditorWindows::GetInstance()->GetWindowUpdate();
		window = {};

		systemManager_->Init();
		scene_->Init();
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
	// 切り替える
	if (ImGui::Button("change")) {
		scene_->SetNextSceneType(changeScene_);
	}
	ImGui::SameLine();
	// 選択する
	int currentIndex = static_cast<int>(changeScene_);
	// ImGui::Comboを使用して選択可能にする
	if (ImGui::Combo("Scene Type", &currentIndex, kSceneTypeNames, IM_ARRAYSIZE(kSceneTypeNames))) {
		// ユーザーが選択を変更した場合にSceneTypeを更新
		changeScene_ = static_cast<SceneType>(currentIndex);
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
	EditorWindows::GetInstance()->Reset();
#endif // _DEBUG

	AOENGINE::PostProcess* postProcess = Engine::GetPostProcess();
	EditorWindows::AddObjectWindow(postProcess, "Post Process");

	AOENGINE::LightGroup* lightGroup = AOENGINE::Render::GetLightGroup();
	lightGroup->GetDirectionalLight()->Reset();
	EditorWindows::AddObjectWindow(lightGroup, "LightGroup");

	AOENGINE::ShadowMap* shadowMap = AOENGINE::Render::GetShadowMap();
	EditorWindows::AddObjectWindow(shadowMap, "ShadowMap");

	auto& window = EditorWindows::GetInstance()->GetWindowUpdate();
	window = {};

	systemManager_->Init();
	scene_->Init();
}

void SceneManager::Free() {
	EditorWindows::GetInstance()->Reset();
	ParticleManager* cpuManager = ParticleManager::GetInstance();
	GpuParticleManager* gpuManager = GpuParticleManager::GetInstance();

	auto& window = EditorWindows::GetInstance()->GetWindowUpdate();
	window = {};

	gpuManager->Finalize();
	cpuManager->Finalize();
	scene_->Finalize();
	systemManager_->Init();
	reset_ = true;
}

bool SceneManager::CheckReset() {
	if (EditorWindows::GetInstance()->GetSceneReset()) {
		return true;
	}
	return false;
}
