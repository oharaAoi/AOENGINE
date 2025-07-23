#include "GameScene.h"
#include "Engine.h"
#include "Engine/Lib/Json/JsonItems.h"
#include "Engine/System/Manager/CollisionLayerManager.h"
#include "Game/Information/ColliderCategory.h"

GameScene::GameScene() {}
GameScene::~GameScene() { Finalize(); }

void GameScene::Finalize() {
	sceneRenderer_->Finalize();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　初期化
//////////////////////////////////////////////////////////////////////////////////////////////////

void GameScene::Init() {
#ifdef _DEBUG
	EditorWindows::GetInstance()->Reset();
#endif

	Engine::GetCanvas2d()->Init();

	JsonItems* adjust = JsonItems::GetInstance();
	adjust->Init("GameScene");

	auto& layers = CollisionLayerManager::GetInstance();
	layers.RegisterCategoryList(GetColliderTagsList());

	PostProcess* postProcess = Engine::GetPostProcess();
	EditorWindows::AddObjectWindow(postProcess, "Post Process");

	LightGroup* lightGroup = Render::GetLightGroup();
	EditorWindows::AddObjectWindow(lightGroup, "LightGroup");

	// -------------------------------------------------
	// ↓ Sceneの初期化
	// -------------------------------------------------
	collisionManager_ = std::make_unique<CollisionManager>();
	collisionManager_->Init();

	sceneLoader_ = SceneLoader::GetInstance();
	sceneLoader_->Init();
	sceneLoader_->Load("./Game/Assets/Scene/", "scene", ".json");

	sceneRenderer_ = SceneRenderer::GetInstance();
	sceneRenderer_->Init();
	sceneRenderer_->CreateObject(sceneLoader_->GetLevelData());
	
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

	floor_ = std::make_unique<Floor>();
	floor_->Init();

	skybox_ = std::make_unique<Skybox>();
	skybox_->Init();
	Render::SetSkyboxTexture(skybox_->GetTexture());

	playerManager_ = std::make_unique<PlayerManager>();
	playerManager_->Init();

	bossRoot_ = std::make_unique<BossRoot>();
	bossRoot_->Init();

	// -------------------------------------------------
	// ↓ managerの初期化
	// -------------------------------------------------

	gameCallBacksManager_ = std::make_unique<GameCallBacksManager>();
	gameCallBacksManager_->SetBossRoot(bossRoot_.get());
	gameCallBacksManager_->SetPlayerManager(playerManager_.get());
	gameCallBacksManager_->SetGround(floor_.get());
	gameCallBacksManager_->Init(collisionManager_.get());

	// -------------------------------------------------
	// ↓ spriteの初期化
	// -------------------------------------------------
	canvas_ = std::make_unique<CanvasUI>();
	canvas_->SetPlayer(playerManager_->GetPlayer());
	canvas_->SetBoss(bossRoot_->GetBoss());
	canvas_->SetFollowCamera(followCamera_.get());
	canvas_->Init();

	fadePanel_ = std::make_unique<FadePanel>();
	fadePanel_->Init();

	// -------------------------------------------------
	// ↓ その他設定
	// -------------------------------------------------
	Player* pPlayer = playerManager_->GetPlayer();
	pPlayer->SetFollowCamera(followCamera_.get());
	pPlayer->SetReticle(canvas_->GetReticle());

	bossRoot_->SetUIs(canvas_->GetBossUIs());

	followCamera_->SetTarget(playerManager_->GetPlayer());
	followCamera_->SetReticle(canvas_->GetReticle());

	DirectionalLight* light = Render::GetLightGroup()->GetDirectionalLight();
	light->SetIntensity(0.5f);

}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　更新
//////////////////////////////////////////////////////////////////////////////////////////////////

void GameScene::Update() {

	// -------------------------------------------------
	// ↓ actorの更新
	// -------------------------------------------------

	skybox_->Update();

	floor_->Update();
	playerManager_->Update();

	bossRoot_->SetPlayerPosition(playerManager_->GetPlayer()->GetGameObject()->GetPosition());
	bossRoot_->Update();

	// -------------------------------------------------
	// ↓ spriteの更新
	// -------------------------------------------------
	canvas_->Update();

	if (canvas_->IsFinishClearNotification()) {
		fadePanel_->SetBlackOut(3.0f);
		fadePanel_->Update();

		if (fadePanel_->GetIsFinished()) {
			nextSceneType_ = SceneType::TITLE;
		}
	}
	
	// -------------------------------------------------
	// ↓ cameraの更新 
	// -------------------------------------------------
	if (debugCamera_->GetIsActive()) {
		debugCamera_->Update();
	} else {
		followCamera_->Update();
	}
	camera2d_->Update();

	// -------------------------------------------------
	// ↓ particleの更新 
	// -------------------------------------------------

	sceneRenderer_->Update();

	collisionManager_->CheckAllCollision();
	gameCallBacksManager_->Update();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　描画
//////////////////////////////////////////////////////////////////////////////////////////////////

void GameScene::Draw() const {
	skybox_->Draw();

	// Sceneの描画
	sceneRenderer_->Draw();

	playerManager_->GetPlayer()->Draw();

	// -------------------------------------------------
	// ↓ spriteの描画
	// -------------------------------------------------
	Engine::SetPipeline(PSOType::Sprite, "Sprite_Normal.json");
	canvas_->Draw();
	fadePanel_->Draw();
}
