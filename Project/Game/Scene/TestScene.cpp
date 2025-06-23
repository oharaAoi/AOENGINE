#include "TestScene.h"
#include "Engine.h"
#include "Engine/System/Input/Input.h"
#include "Engine/Lib/Json/JsonItems.h"
#include "Engine/System/Editer/Window/EditorWindows.h"
#include "Engine/System/ParticleSystem/Tool/EffectSystem.h"
#include "Engine/Module/Geometry/Polygon/PlaneGeometry.h"

TestScene::TestScene() {}
TestScene::~TestScene() { particleManager_->Finalize(); }

void TestScene::Finalize() {
}

void TestScene::Init() {
	JsonItems* adjust = JsonItems::GetInstance();
	adjust->Init("TestScene");

	// カメラ -------------------------------------------------------------------
	camera2d_ = std::make_unique<Camera2d>();
	camera3d_ = std::make_unique<Camera3d>();
	debugCamera_ = std::make_unique<DebugCamera>();

	camera2d_->Init();
	camera3d_->Init();
	debugCamera_->Init();

	// worldObject -------------------------------------------------------------------
	skydome_ = std::make_unique<Skydome>();
	skydome_->Init();

	floor_ = std::make_unique<Floor>();
	floor_->Init();

	// gameObject -------------------------------------------------------------------
	for (uint32_t oi = 0; oi < kObjectNum_; ++oi) {
		testObjA_[oi] = std::make_unique<TestObject>();

		testObjA_[oi]->Init();
		testObjA_[oi]->SetCollider("TestObj", ColliderShape::SPHERE);
	}

	plane_ = std::make_unique<GeometryObject>();
	plane_->Set<CubeGeometry>();

	particleManager_ = ParticleManager::GetInstance();
	particleManager_->Init();

	// Manager -------------------------------------------------------------------

	collisionManager_ = std::make_unique<CollisionManager>();
	collisionManager_->Init();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// 更新
//////////////////////////////////////////////////////////////////////////////////////////////////
void TestScene::Update() {
	// -------------------------------------------------
	// ↓ カメラの更新
	// -------------------------------------------------
	camera2d_->Update();
	camera3d_->Update();
	if (isDebugCamera_) {
		debugCamera_->Update();
		
		EffectSystem::GetInstacne()->SetCameraMatrix(debugCamera_->GetCameraMatrix());
		EffectSystem::GetInstacne()->SetViewProjectionMatrix(debugCamera_->GetViewMatrix(), debugCamera_->GetProjectionMatrix());
	} else {
		
		EffectSystem::GetInstacne()->SetCameraMatrix(camera3d_->GetCameraMatrix());
		EffectSystem::GetInstacne()->SetViewProjectionMatrix(camera3d_->GetViewMatrix(), camera3d_->GetProjectionMatrix());
	}

	// -------------------------------------------------
	// ↓ worldObjectの更新
	// -------------------------------------------------
	skydome_->Update();
	floor_->Update();

	// -------------------------------------------------
	// ↓ GameObjectの更新
	// -------------------------------------------------

	plane_->Update();
	
	for (uint32_t oi = 0; oi < kObjectNum_; ++oi) {
		testObjA_[oi]->Update();
	}
	
	collisionManager_->CheckAllCollision();

	// -------------------------------------------------
	// ↓ ParticleのViewを設定する
	// -------------------------------------------------

	
	particleManager_->SetView(debugCamera_->GetViewMatrix() * debugCamera_->GetProjectionMatrix(), Matrix4x4::MakeUnit());
	particleManager_->PostUpdate();
}

void TestScene::Draw() const {
	Engine::SetPipeline(PSOType::Object3d, "Object_Normal.json");
	skydome_->Draw();

	for (uint32_t oi = 0; oi < kObjectNum_; ++oi) {
		testObjA_[oi]->Draw();
	}
	//plane_->Draw();

	Engine::SetPipeline(PSOType::Object3d, "Object_Particle.json");
	ParticleManager::GetInstance()->Draw();
}
