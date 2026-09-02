#include "TestScene.h"

#include "Engine/Core/Engine.h"
#include "Engine/Render/Render.h"
#include "Engine/Module/Components/GameObject/BaseGameObject.h"

#include "Engine/Utilities/SceneObjectFinder.h"
#include "Game/Actor/Player/Player.h"
#include "Game/Camera/FollowCamera.h"

TestScene::TestScene() {}
TestScene::~TestScene() { Finalize(); }

void TestScene::Finalize() {
	followCamera_.reset();
	player_.reset();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// 初期化
//////////////////////////////////////////////////////////////////////////////////////////////////
void TestScene::Init() {
	AOENGINE::Render::GetLightGroup()->Load();

	player_ = std::make_unique<Player>();

	followCamera_ = std::make_unique<FollowCamera>();
	followCamera_->Init();

	playerResolved_ = false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// Play開始時 (シーンJSON読み込み後)
//////////////////////////////////////////////////////////////////////////////////////////////////
void TestScene::OnPlayStart() {
	TryAcquirePlayer();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// 更新
//////////////////////////////////////////////////////////////////////////////////////////////////
void TestScene::Update() {
	// OnPlayStartが呼ばれない経路(Editorのシーンリセット等)への保険として
	// 取得できるまで毎フレーム試す
	if (!playerResolved_) {
		TryAcquirePlayer();
	}

	if (player_) {
		player_->Update();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// Updateより後の更新 (プレイヤーが動いた後にカメラを追従させる)
//////////////////////////////////////////////////////////////////////////////////////////////////
void TestScene::PostUpdate() {
	if (followCamera_) {
		followCamera_->Update();	// 追従対象はFollowCamera内部で自動取得する
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// 描画
//////////////////////////////////////////////////////////////////////////////////////////////////
void TestScene::Draw() const {
	if (!followCamera_ || !followCamera_->HasTarget()) {
		// まだPlayerが居ないときは通常のCamera3dで描画する
		BaseScene::Draw();
		return;
	}

	// BaseScene::Draw() の Game Camera を FollowCamera へ差し替えたもの
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

//////////////////////////////////////////////////////////////////////////////////////////////////
// "Player" の取得 (Player エンティティ用。FollowCamera は自分で取得する)
//////////////////////////////////////////////////////////////////////////////////////////////////
void TestScene::TryAcquirePlayer() {
	AOENGINE::BaseGameObject* body = FindSceneObject<AOENGINE::BaseGameObject>("Player");
	if (!body) {
		return;
	}

	player_->Init(body);
	playerResolved_ = true;
}
