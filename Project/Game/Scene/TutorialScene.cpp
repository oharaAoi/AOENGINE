#include "TutorialScene.h"
#include "Engine.h"
#include "Engine/Lib/Json/JsonItems.h"
#include "Engine/System/Manager/CollisionLayerManager.h"
#include "Game/Information/ColliderCategory.h"
#include "Game/UI/Guide/TutorialBehavior.h"

TutorialScene::TutorialScene() {}
TutorialScene::~TutorialScene() { Finalize(); }

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　終了処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void TutorialScene::Finalize() {
	sceneRenderer_->Finalize();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　初期化
//////////////////////////////////////////////////////////////////////////////////////////////////

void TutorialScene::Init() {
	Engine::GetCanvas2d()->Init();

	JsonItems* adjust = JsonItems::GetInstance();
	adjust->Init("TutorialScene");

	auto& layers = CollisionLayerManager::GetInstance();
	layers.RegisterCategoryList(GetColliderTagsList());

	// -------------------------------------------------
	// ↓ Sceneの初期化
	// -------------------------------------------------
	collisionManager_ = std::make_unique<CollisionManager>();
	collisionManager_->Init();

	sceneLoader_ = SceneLoader::GetInstance();
	sceneLoader_->Init();
	sceneLoader_->Load("./Packages/Game/Assets/Scene/", "tutorial", ".json");

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
	canvas_->Init(true);

	tutorialMissionGauge_ = std::make_unique<TutorialMissionGauge>();
	tutorialMissionGauge_->Init();

	fadePanel_ = std::make_unique<FadePanel>();
	fadePanel_->Init();

	// -------------------------------------------------
	// ↓ その他設定
	// -------------------------------------------------
	ChangeBehavior(new TutorialMoveBehavior(this));

	Player* pPlayer = playerManager_->GetPlayer();
	pPlayer->SetFollowCamera(followCamera_.get());
	pPlayer->SetReticle(canvas_->GetReticle());

	bossRoot_->SetUIs(canvas_->GetBossUIs());
	bossRoot_->SetPlayer(playerManager_->GetPlayer());

	followCamera_->SetTarget(playerManager_->GetPlayer());
	followCamera_->SetReticle(canvas_->GetReticle());

	DirectionalLight* light = Render::GetLightGroup()->GetDirectionalLight();
	light->SetIntensity(0.5f);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　更新
//////////////////////////////////////////////////////////////////////////////////////////////////

void TutorialScene::Update() {
	// -------------------------------------------------
	// ↓ actorの更新
	// -------------------------------------------------

	playerManager_->Update();
	bossRoot_->GetBoss()->GetBehaviorTree()->SetExecute(false);
	bossRoot_->Update();

	tutorialMissionGauge_->Update();
	tutorialBehavior_->Update();

	if (Input::GetInstance()->IsTriggerButton(XInputButtons::START)) {
		nextSceneType_ = SceneType::GAME;
	}

	// -------------------------------------------------
	// ↓ spriteの更新
	// -------------------------------------------------
	canvas_->Update();

	fadePanel_->Update();

	if (fadePanel_->GetIsFinished()) {
		nextSceneType_ = SceneType::GAME;
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

void TutorialScene::Draw() const {
	sceneRenderer_->Draw();
}

void TutorialScene::ChangeBehavior(ITutorialBehavior* _newBehavior) {
	tutorialBehavior_.reset(_newBehavior);
	if (tutorialBehavior_) {
		tutorialBehavior_->Init();
	}
}

void TutorialScene::ToGameScene() {
	fadePanel_->SetBlackOut(3.0f);
}
