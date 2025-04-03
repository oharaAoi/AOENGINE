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
	camera2d_ = std::make_unique<Camera2d>();
	followCamera_->Init();
	debugCamera_->Init();
	camera2d_->Init();
	
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
	// ↓ Spriteの初期化
	// -------------------------------------------------
	reticle_ = std::make_unique<Reticle>();
	reticle_->Init();

	// -------------------------------------------------
	// ↓ その他設定
	// -------------------------------------------------
	Player* pPlayer = playerManager_->GetPlayer();
	pPlayer->SetFollowCamera(followCamera_.get());
	pPlayer->SetReticle(reticle_.get());

	followCamera_->SetTarget(playerManager_->GetPlayer());

}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　更新
//////////////////////////////////////////////////////////////////////////////////////////////////

void GameScene::Update() {

	// -------------------------------------------------
	// ↓ actorの更新
	// -------------------------------------------------
	skydome_->Update();
	playerManager_->Update();
	boss_->Update();

	// -------------------------------------------------
	// ↓ spriteの更新
	// -------------------------------------------------
	reticle_->Update(boss_->GetTransform()->GetWorldMatrix(), followCamera_->GetVpvpMatrix());

	// -------------------------------------------------
	// ↓ cameraの更新 
	// -------------------------------------------------
	if (debugCamera_->GetIsActive()) {
		debugCamera_->Update();
	} else {
		followCamera_->Update();
	}
	camera2d_->Update();
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

	// -------------------------------------------------
	// ↓ spriteの描画
	// -------------------------------------------------
	Engine::SetPipeline(PipelineType::SpriteNormalBlendPipeline);
	reticle_->Draw();

}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　Debug表示
//////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
void GameScene::Debug_Gui() {
	
}

#endif