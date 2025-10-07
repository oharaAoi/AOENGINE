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
	Engine::GetCanvas2d()->Init();

	JsonItems* adjust = JsonItems::GetInstance();
	adjust->Init("GameScene");

	auto& layers = CollisionLayerManager::GetInstance();
	layers.RegisterCategoryList(GetColliderTagsList());

	// -------------------------------------------------
	// ↓ Sceneの初期化
	// -------------------------------------------------
	collisionManager_ = std::make_unique<CollisionManager>();
	collisionManager_->Init();

	sceneLoader_ = SceneLoader::GetInstance();
	sceneLoader_->Init();
	sceneLoader_->Load("./Packages/Game/Assets/Scene/", "scene", ".json");

	sceneRenderer_ = SceneRenderer::GetInstance();
	sceneRenderer_->Init();
	sceneRenderer_->CreateObject(sceneLoader_->GetLevelData());
	EditorWindows::GetInstance()->SetSceneRenderer(sceneRenderer_);

	skybox_ = SceneRenderer::GetInstance()->AddObject<Skybox>("Skybox", "Object_Skybox.json", -999);
	Render::SetSkyboxTexture(skybox_->GetTexture());

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
	canvas_->Init(false);

	fadePanel_ = std::make_unique<FadePanel>();
	fadePanel_->Init();

	// -------------------------------------------------
	// ↓ その他設定
	// -------------------------------------------------
	Player* pPlayer = playerManager_->GetPlayer();
	pPlayer->SetFollowCamera(followCamera_.get());
	pPlayer->SetReticle(canvas_->GetReticle());

	bossRoot_->SetUIs(canvas_->GetBossUIs());
	bossRoot_->SetPlayer(playerManager_->GetPlayer());

	followCamera_->SetTarget(playerManager_->GetPlayer());
	followCamera_->SetReticle(canvas_->GetReticle());

	DirectionalLight* light = Render::GetLightGroup()->GetDirectionalLight();
	light->SetIntensity(0.5f);

	emitter_ = GpuParticleManager::GetInstance()->CreateEmitter("filed");
	field_ = GpuParticleManager::GetInstance()->CreateField("Window");

}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　更新
//////////////////////////////////////////////////////////////////////////////////////////////////

void GameScene::Update() {

	// -------------------------------------------------
	// ↓ actorの更新
	// -------------------------------------------------
	
	playerManager_->Update();

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

	playerManager_->PostUpdate();

	sceneRenderer_->PostUpdate();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　描画
//////////////////////////////////////////////////////////////////////////////////////////////////

void GameScene::Draw() const {
	// Sceneの描画
	sceneRenderer_->Draw();
}
