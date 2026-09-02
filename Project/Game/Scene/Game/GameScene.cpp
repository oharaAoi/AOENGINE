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
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// スタート時に呼ばれる処理(パラメータの読み込み、設定などはココで行う)
//////////////////////////////////////////////////////////////////////////////////////////////////

void GameScene::OnPlayStart() {
	// Player初期化
	AOENGINE::BaseGameObject* body = FindSceneObject<AOENGINE::BaseGameObject>("Player");
	player_->Init(body);
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
	pSceneRenderer_->DrawShadowMap();
	followCamera_->ApplyToRender();
	Engine::BeginSceneView(SceneViewType::Game);
	skybox_->Draw();
	pSceneRenderer_->DrawSceneObjects();

#ifdef _DEVELOPMENT
	debugCamera_->ApplyToRender();
	Engine::BeginSceneView(SceneViewType::Editor);
	skybox_->Draw();
	pSceneRenderer_->DrawSceneObjects();
#endif

	Engine::ActivateSceneView(SceneViewType::Game);
}
