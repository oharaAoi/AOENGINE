#include "GameScene.h"

#include "Engine/Core/Engine.h"
#include "Engine/Render/Render.h"
#include "Engine/Module/Components/GameObject/BaseGameObject.h"
#include "Engine/Utilities/SceneObjectFinder.h"

#include "Game/Actor/Player/Player.h"
#include "Game/Actor/Boss/Boss.h"
#include "Game/Camera/FollowCamera.h"

GameScene::GameScene() {}

GameScene::~GameScene() {
	Finalize();
}

void GameScene::Finalize() {
	boss_.reset();
	followCamera_.reset();
	player_.reset();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// 初期化処理(インスタンスの宣言など)
//////////////////////////////////////////////////////////////////////////////////////////////////

void GameScene::Init() {
	AOENGINE::Render::GetLightGroup()->Load();

	player_ = std::make_unique<Player>();
	followCamera_ = std::make_unique<FollowCamera>();
	boss_ = std::make_unique<Boss>();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// スタート時に呼ばれる処理(パラメータの読み込み、設定などはココで行う)
//////////////////////////////////////////////////////////////////////////////////////////////////

void GameScene::OnPlayStart() {
	// シーンに配置済みの本体を名前で取得して各エンティティに束ねる
	player_->Init(FindSceneObject<AOENGINE::BaseGameObject>("Player"));
	boss_->Init(FindSceneObject<AOENGINE::BaseGameObject>("Boss"));
	followCamera_->Init();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// 更新
//////////////////////////////////////////////////////////////////////////////////////////////////

void GameScene::Update() {

	// プレイヤー
	if (player_) {
		player_->Update();
	}

	// フォローカメラ
	if (followCamera_) {
		followCamera_->Update();
	}

	// ボスの更新
	if (boss_ && followCamera_) {
		const Math::Matrix4x4 viewProjection =
			followCamera_->GetViewMatrix() * followCamera_->GetProjectionMatrix();
		boss_->Update(viewProjection);
	}

#ifdef _DEVELOPMENT
	// 調整パラメータの編集
	ImGui::Begin("GameScene Parameters");

	if (player_ && ImGui::CollapsingHeader("Player")) {
		ImGui::PushID("Player");
		player_->Debug_Gui();
		ImGui::PopID();
	}
	if (followCamera_ && ImGui::CollapsingHeader("FollowCamera")) {
		ImGui::PushID("FollowCamera");
		followCamera_->Debug_Gui();
		ImGui::PopID();
	}
	if (boss_ && ImGui::CollapsingHeader("Boss")) {
		ImGui::PushID("Boss");
		boss_->Debug_Gui();
		ImGui::PopID();
	}

	ImGui::End();
#endif
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// 描画
//////////////////////////////////////////////////////////////////////////////////////////////////

void GameScene::Draw() const {
	if (!followCamera_ || !followCamera_->HasTarget()) {
		// Playerが居ないときは通常のCamera3dで描画する
		BaseScene::Draw();
		return;
	}

	// BaseScene::Draw()のGameCameraをFollowCameraに差し替えたもの
	if (pSceneRenderer_) {
		pSceneRenderer_->DrawShadowMap();

		// FollowCamera から描画する
		Engine::BeginSceneView(SceneViewType::Game);
		followCamera_->ApplyToRender();
		Engine::CommitSceneViewCamera(SceneViewType::Game);
		skybox_->Draw();
		pSceneRenderer_->DrawSceneObjects(
			followCamera_->GetViewMatrix() * followCamera_->GetProjectionMatrix());

#ifdef _DEVELOPMENT

		Engine::BeginSceneView(SceneViewType::Editor);
		debugCamera_->ApplyToRender();
		Engine::CommitSceneViewCamera(SceneViewType::Editor);
		skybox_->Draw();
		pSceneRenderer_->DrawSceneObjects(
			debugCamera_->GetViewMatrix() * debugCamera_->GetProjectionMatrix());
#endif
	}

	// PostProcess/PostDrawはGame View に対して実行される
	Engine::ActivateSceneView(SceneViewType::Game);
}
