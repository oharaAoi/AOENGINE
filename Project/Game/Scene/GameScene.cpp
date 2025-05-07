#include "GameScene.h"
#include "Engine.h"
#include "Engine/Lib/Json/JsonItems.h"
#include "Engine/System/Manager/CollisionLayerManager.h"
#include "Game/Information/ColliderCategory.h"

GameScene::GameScene() {}
GameScene::~GameScene() { Finalize(); }

void GameScene::Finalize() {
	particleManager_->Finalize();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　初期化
//////////////////////////////////////////////////////////////////////////////////////////////////

void GameScene::Init() {
	JsonItems* adjust = JsonItems::GetInstance();
	adjust->Init("GameScene");

	auto& layers = CollisionLayerManager::GetInstance();
	layers.RegisterCategoryList(GetColliderTagsList());

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
	// ↓ Particleの初期化
	// -------------------------------------------------
	particleManager_ = ParticleManager::GetInstance();
	particleManager_->Init();
	
	// -------------------------------------------------
	// ↓ actorの初期化
	// -------------------------------------------------
	
	skydome_ = std::make_unique<Skydome>();
	skydome_->Init();

	playerManager_ = std::make_unique<PlayerManager>();
	playerManager_->Init();

	boss_ = std::make_unique<Boss>();
	boss_->Init();

	cylinder_ = std::make_unique<GeometryObject>();
	cylinder_->Set<CylinderGeometry>(32, 1.f, 2.f, 2.f);
	cylinder_->SetEditorWindow();

	sphere_ = std::make_unique<GeometryObject>();
	sphere_->Set<SphereGeometry>();
	sphere_->SetEditorWindow();

	// -------------------------------------------------
	// ↓ managerの初期化
	// -------------------------------------------------

	collisionManager_ = std::make_unique<CollisionManager>();
	collisionManager_->Init();

	gameCallBacksManager_ = std::make_unique<GameCallBacksManager>();
	gameCallBacksManager_->SetBoss(boss_.get());
	gameCallBacksManager_->SetPlayerManager(playerManager_.get());
	gameCallBacksManager_->Init(collisionManager_.get());

	// -------------------------------------------------
	// ↓ spriteの初期化
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
	followCamera_->SetReticle(reticle_.get());

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

	cylinder_->Update();
	sphere_->Update();

	collisionManager_->CheckAllCollision();
	gameCallBacksManager_->Update();

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

	// -------------------------------------------------
	// ↓ particleの更新 
	// -------------------------------------------------
	particleManager_->SetView(followCamera_->GetViewMatrix() * followCamera_->GetProjectionMatrix(), Matrix4x4::MakeUnit());
	particleManager_->PostUpdate();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　描画
//////////////////////////////////////////////////////////////////////////////////////////////////

void GameScene::Draw() const {
	Engine::SetPSOPrimitive();
	if (debugCamera_->GetIsActive()) {
		DrawGrid(debugCamera_->GetViewMatrix(), debugCamera_->GetProjectionMatrix());
	} else {
		DrawGrid(followCamera_->GetViewMatrix(), followCamera_->GetProjectionMatrix());
	}
	Engine::SetPSOObj(Object3dPSO::Normal);
	skydome_->Draw();
	playerManager_->Draw();
	boss_->Draw();

	// -------------------------------------------------
	// ↓ spriteの描画
	// -------------------------------------------------
	Engine::SetPSOSprite(SpritePSO::Normal);
	reticle_->Draw();

	Engine::SetPSOObj(Object3dPSO::Particle);
	ParticleManager::GetInstance()->Draw();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　Debug表示
//////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
void GameScene::Debug_Gui() {
	
}

#endif