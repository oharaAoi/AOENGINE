#include "GameScene.h"
#include "Engine.h"
#include "Engine/Lib/Json/JsonItems.h"
#include "Engine/System/Manager/CollisionLayerManager.h"
#include "Game/Information/ColliderCategory.h"
#include "Game/Scene/SceneBehavior/GameSceneBehavior.h"

GameScene::GameScene() {}
GameScene::~GameScene() { Finalize(); }

void GameScene::Finalize() {

}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　初期化
//////////////////////////////////////////////////////////////////////////////////////////////////

void GameScene::Init() {
	Engine::GetCanvas2d()->Init();

	AOENGINE::JsonItems* adjust = AOENGINE::JsonItems::GetInstance();
	adjust->Init("GameScene");

	auto& layers = AOENGINE::CollisionLayerManager::GetInstance();
	layers.RegisterCategoryList(GetColliderTagsList());

	AOENGINE::Render::GetLightGroup()->Load();

	// -------------------------------------------------
	// ↓ Sceneの初期化
	// -------------------------------------------------

	LoadScene("./Project/Packages/Game/Assets/Scene/", "TL", ".json");

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
	canvas_->Init(false);

	fadePanel_ = std::make_unique<FadePanel>();
	fadePanel_->Init();
	fadePanel_->SetColor(AOENGINE::Color(0, 0, 0, 0));

	ChangeBehavior(new GamePlayBehavior(this));

	// -------------------------------------------------
	// ↓ その他設定
	// -------------------------------------------------
	Player* pPlayer = playerManager_->GetPlayer();
	pPlayer->SetFollowCamera(followCamera_.get());
	pPlayer->SetReticle(canvas_->GetReticle());

	bossRoot_->SetUIs(canvas_->GetBossUIs());
	bossRoot_->SetPlayer(playerManager_->GetPlayer());
	bossRoot_->SetTargetTransform(playerManager_->GetPlayer()->GetTransform());
	bossRoot_->SetFollowCamera(followCamera_.get());

	followCamera_->SetTarget(playerManager_->GetPlayer());
	followCamera_->SetReticle(canvas_->GetReticle());

	emitter_ = AOENGINE::GpuParticleManager::GetInstance()->CreateEmitter("rain");
	field_ = AOENGINE::GpuParticleManager::GetInstance()->CreateField("Window");
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　更新
//////////////////////////////////////////////////////////////////////////////////////////////////

void GameScene::Update() {
	gameSceneBehavior_->Update();

	// -------------------------------------------------
	// ↓ spriteの更新
	// -------------------------------------------------
	canvas_->Update();

	if (canvas_->IsFinishClearNotification()) {
		fadePanel_->SetBlackOut(3.0f);
		fadePanel_->Update();

		if (fadePanel_->GetIsFinished()) {
			nextSceneType_ = SceneType::Title;
		}
	}

	// -------------------------------------------------
	// ↓ playerの死亡確認
	// -------------------------------------------------
	if (playerManager_->CheckIsDead()) {
		fadePanel_->SetBlackOut(3.0f);
		fadePanel_->Update();

		if (fadePanel_->GetIsFinished()) {
			nextSceneType_ = SceneType::Title;
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
}

void GameScene::PostUpdate() {
	gameCallBacksManager_->Update();
	playerManager_->PostUpdate();
}

void GameScene::ChangeBehavior(IGameSceneBehavior* _newBehavior) {
	gameSceneBehavior_.reset(_newBehavior);
	if (gameSceneBehavior_) {
		gameSceneBehavior_->Init();
	}
}

void GameScene::CharactorUpdate() {
	playerManager_->Update();

	bossRoot_->Update();
}
