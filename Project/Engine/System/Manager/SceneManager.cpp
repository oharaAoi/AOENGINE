#include "SceneManager.h"
#include <optional>
#include "Engine/System/Manager/ParticleManager.h"

SceneManager::SceneManager() {}

SceneManager::~SceneManager() {}

void SceneManager::Finalize() {
	effectSystem_->Finalize();
	scene_.reset();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　初期化処理
//////////////////////////////////////////////////////////////////////////////////////////////////
void SceneManager::Init() {
	// gameに必要なResourceの読み込み
	sceneFactory_ = std::make_unique<SceneFactory>();

	effectSystem_ = EffectSystem::GetInstacne();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　更新処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void SceneManager::Update() {
	if (scene_->GetNextSceneType()) {
		SetChange(scene_->GetNextSceneType().value());
		scene_->SetNextSceneType(std::nullopt);
	}

	if (EditorWindows::GetInstance()->GetSceneReset()) {
		EditorWindows::GetInstance()->Reset();
		ParticleManager::GetInstance()->Finalize();
		ParticleManager::GetInstance()->Init();
		scene_->Init();
		return;
	}

	scene_->Update();
	effectSystem_->Update();

	SceneRenderer::GetInstance()->PostUpdate();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　描画処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void SceneManager::Draw() {
	scene_->Draw();
	effectSystem_->Draw();
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
	nextScene_ = sceneFactory_->CreateScene(sceneFactory_->SceneTypeToString(type));
	scene_ = std::move(nextScene_);
	scene_->Init();
}