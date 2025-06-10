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

	sceneRenderer_ = SceneRenderer::GetInstance();
	sceneRenderer_->Init();

	sceneLoader_ = SceneLoader::GetInstance();
	sceneLoader_->Init();
	sceneLoader_->Load("./Game/Assets/Scene/", "scene", ".json");
	
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

	floor_ = std::make_unique<Floor>();
	floor_->Init();

	skybox_ = std::make_unique<Skybox>();
	skybox_->Init();
	Render::SetSkyboxTexture(skybox_->GetTexture());

	playerManager_ = std::make_unique<PlayerManager>();
	playerManager_->Init();

	bossRoot_ = std::make_unique<BossRoot>();
	bossRoot_->Init();

	cylinder_ = std::make_unique<GeometryObject>();
	cylinder_->Set<CylinderGeometry>(32, 1.f, 2.f, 2.f);
	//cylinder_->SetEditorWindow();

	sphere_ = std::make_unique<GeometryObject>();
	sphere_->Set<SphereGeometry>();
	sphere_->SetEditorWindow();

	hitExploade_ = std::make_unique<HitExplode>();
	hitExploade_->Init();


	// -------------------------------------------------
	// ↓ managerの初期化
	// -------------------------------------------------

	collisionManager_ = std::make_unique<CollisionManager>();
	collisionManager_->Init();

	gameCallBacksManager_ = std::make_unique<GameCallBacksManager>();
	gameCallBacksManager_->SetBossRoot(bossRoot_.get());
	gameCallBacksManager_->SetPlayerManager(playerManager_.get());
	gameCallBacksManager_->SetGround(floor_.get());
	gameCallBacksManager_->Init(collisionManager_.get());

	// -------------------------------------------------
	// ↓ spriteの初期化
	// -------------------------------------------------
	canvas_ = std::make_unique<CanvasUI>();
	canvas_->Init();

	// -------------------------------------------------
	// ↓ その他設定
	// -------------------------------------------------
	Player* pPlayer = playerManager_->GetPlayer();
	pPlayer->SetFollowCamera(followCamera_.get());
	pPlayer->SetReticle(canvas_->GetReticle());

	followCamera_->SetTarget(playerManager_->GetPlayer());
	followCamera_->SetReticle(canvas_->GetReticle());

	canvas_->SetPlayer(playerManager_->GetPlayer());
	canvas_->SetBoss(bossRoot_->GetBoss());
	canvas_->SetFollowCamera(followCamera_.get());
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　更新
//////////////////////////////////////////////////////////////////////////////////////////////////

void GameScene::Update() {

	// -------------------------------------------------
	// ↓ actorの更新
	// -------------------------------------------------

	skybox_->Update();

	skydome_->Update();
	floor_->Update();
	playerManager_->Update();

	bossRoot_->SetPlayerPosition(playerManager_->GetPlayer()->GetPosition());
	bossRoot_->Update();

	cylinder_->Update();
	sphere_->Update();

	collisionManager_->CheckAllCollision();
	gameCallBacksManager_->Update();

	hitExploade_->Update();

	// -------------------------------------------------
	// ↓ spriteの更新
	// -------------------------------------------------
	canvas_->Update();
	
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
	particleManager_->Update();
	particleManager_->SetView(followCamera_->GetViewMatrix() * followCamera_->GetProjectionMatrix(), Matrix4x4::MakeUnit());
	particleManager_->PostUpdate();

	sceneRenderer_->Update();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　描画
//////////////////////////////////////////////////////////////////////////////////////////////////

void GameScene::Draw() const {
	Engine::SetPSOPrimitive();
	/*if (debugCamera_->GetIsActive()) {
		DrawGrid(debugCamera_->GetViewMatrix(), debugCamera_->GetProjectionMatrix());
	} else {
		DrawGrid(followCamera_->GetViewMatrix(), followCamera_->GetProjectionMatrix());
	}*/

	skybox_->Draw();

	// Sceneの描画
	sceneRenderer_->Draw();

	Engine::SetPipeline(PSOType::Object3d, "Object_NormalEnviroment.json");
	//sphere_->Draw();

	
	Engine::SetPipeline(PSOType::Object3d, "Object_Particle.json");
	ParticleManager::GetInstance()->Draw();

	gameCallBacksManager_->Draw();

	// -------------------------------------------------
	// ↓ spriteの描画
	// -------------------------------------------------
	Engine::SetPipeline(PSOType::Sprite, "Sprite_Normal.json");
	canvas_->Draw();

}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　Debug表示
//////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
void GameScene::Debug_Gui() {
	
}

#endif