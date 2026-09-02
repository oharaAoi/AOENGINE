#include "GameScene.h"

#include "Engine/Core/Engine.h"
#include "Engine/Render/Render.h"
#include "Engine/Module/Components/GameObject/BaseGameObject.h"
#include "Engine/Utilities/SceneObjectFinder.h"

#include "Game/Actor/Player/Player.h"
#include "Game/Camera/FollowCamera.h"

GameScene::GameScene() {}

GameScene::~GameScene() {
	Finalize();
}

void GameScene::Finalize() {
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
	followCamera_->Init();

	backgrounds_ = std::make_unique<StageBackgrounds>();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// スタート時に呼ばれる処理(パラメータの読み込み、設定などはココで行う)
//////////////////////////////////////////////////////////////////////////////////////////////////

void GameScene::OnPlayStart() {
	// Player初期化
	AOENGINE::BaseGameObject* body = FindSceneObject<AOENGINE::BaseGameObject>("Player");
	player_->Init(body);

	backgrounds_->Init();
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

	// 背景
	backgrounds_->Update(player_->GetPosition());

#ifdef _DEVELOPMENT
	// 調整パラメータの編集 + Save/Load
	if (player_) {
		ImGui::Begin("Player");
		player_->Debug_Gui();
		ImGui::End();
	}
	if (followCamera_) {
		ImGui::Begin("FollowCamera");
		followCamera_->Debug_Gui();
		ImGui::End();
	}
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
		Engine::GetCanvas2d()->Update();
		Engine::DrawBackgroundSprites(SceneViewType::Game);
		// 背景Sprite用の単一RTから3D用MRTへ、内容を保持したまま戻す。
		Engine::BeginSceneView(SceneViewType::Game, false);
		pSceneRenderer_->DrawSceneObjects(
			followCamera_->GetViewMatrix() * followCamera_->GetProjectionMatrix());

#ifdef _DEVELOPMENT

		Engine::BeginSceneView(SceneViewType::Editor);
		debugCamera_->ApplyToRender();
		Engine::CommitSceneViewCamera(SceneViewType::Editor);
		skybox_->Draw();
		Engine::DrawBackgroundSprites(SceneViewType::Editor);
		Engine::BeginSceneView(SceneViewType::Editor, false);
		pSceneRenderer_->DrawSceneObjects(
			debugCamera_->GetViewMatrix() * debugCamera_->GetProjectionMatrix());
#endif
	}

	// PostProcess/PostDrawはGame View に対して実行される
	Engine::ActivateSceneView(SceneViewType::Game);
}
