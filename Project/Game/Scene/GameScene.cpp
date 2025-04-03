#include "GameScene.h"
#include "Engine.h"
#include "Engine/Lib/Json/JsonItems.h"

GameScene::GameScene() {}
GameScene::~GameScene() {}

void GameScene::Finalize() {
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　初期化
//////////////////////////////////////////////////////////////////////////////////////////////////

void GameScene::Init() {
	JsonItems* adjust = JsonItems::GetInstance();
	adjust->Init("GameScene");

	// -------------------------------------------------
	// ↓ cameraの初期化
	// -------------------------------------------------
	
	followCamera_ = std::make_unique<FollowCamera>();
	debugCamera_ = std::make_unique<DebugCamera>();
	followCamera_->Init();
	debugCamera_->Init();
	
	// -------------------------------------------------
	// ↓ actorの初期化
	// -------------------------------------------------
	
	skydome_ = std::make_unique<Skydome>();
	skydome_->Init();

	playerManager_ = std::make_unique<PlayerManager>();
	playerManager_->Init();

	boss_ = std::make_unique<Boss>();
	boss_->Init();

	// -------------------------------------------------
	// ↓ その他設定
	// -------------------------------------------------

	playerManager_->GetPlayer()->SetFollowCamera(followCamera_.get());
	followCamera_->SetTarget(playerManager_->GetPlayer());

}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　更新
//////////////////////////////////////////////////////////////////////////////////////////////////

void GameScene::Update() {

	// actorの更新 -----------------------------------------
	skydome_->Update();
	playerManager_->Update();
	boss_->Update();

	// cameraの更新 -----------------------------------------
	if (debugCamera_->GetIsActive()) {
		debugCamera_->Update();
	} else {
		followCamera_->Update();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　描画
//////////////////////////////////////////////////////////////////////////////////////////////////

void GameScene::Draw() const {
	Engine::SetPipeline(PipelineType::PrimitivePipeline);
	if (debugCamera_->GetIsActive()) {
		DrawGrid(debugCamera_->GetViewMatrix(), debugCamera_->GetProjectionMatrix());
	} else {
		DrawGrid(followCamera_->GetViewMatrix(), followCamera_->GetProjectionMatrix());
	}
	Engine::SetPipeline(PipelineType::NormalPipeline);
	skydome_->Draw();
	playerManager_->Draw();
	boss_->Draw();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　Debug表示
//////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
void GameScene::Debug_Gui() {
	
}

#endif