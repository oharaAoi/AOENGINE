#include "TestScene.h"
#include "Engine.h"
#include "Engine/System/Input/Input.h"
#include "Engine/Lib/Json/JsonItems.h"
#include "Engine/System/Editer/Window/EditerWindows.h"
#include "Engine/System/ParticleSystem/Tool/EffectSystem.h"
#include "Engine/Geometry/Polygon/PlaneGeometry.h"

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

	particle = std::make_unique<TestParticle>();
	particle->Init();
	
#ifdef _DEBUG
	//EditerWindows::AddObjectWindow(testObjA_.get(), "testAObj");
#endif

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
	
	collisionManager_->Reset();
	//collisionManager_->AddCollider(testObjA_->GetCollider());
	collisionManager_->CheckAllCollision();

	// -------------------------------------------------
	// ↓ ParticleのViewを設定する
	// -------------------------------------------------

	particle->Update(debugCamera_->GetRotate());

	particleManager_->SetView(debugCamera_->GetViewMatrix() * debugCamera_->GetProjectionMatrix(), Matrix4x4::MakeUnit());
	particleManager_->PostUpdate();
}

void TestScene::Draw() const {

	if (isDebugCamera_) {
		DrawGrid(debugCamera_->GetViewMatrix(), debugCamera_->GetProjectionMatrix());
	} else {
		DrawGrid(camera3d_->GetViewMatrix(), camera3d_->GetProjectionMatrix());
	}

	Engine::SetPSOObj(Object3dPSO::Normal);
	skydome_->Draw();

	/*for (uint32_t oi = 0; oi < kObjectNum_; ++oi) {
		testObjA_[oi]->Draw();
	}*/

	Engine::SetPSOObj(Object3dPSO::Particle);
	particleManager_->Draw();
	//plane_->Draw();
}

#ifdef _DEBUG
void TestScene::Debug_Gui() {
	ImGui::Checkbox("isDebug", &isDebugCamera_);

	/*if (ImGui::Button("reserve")) {
		testObjA_->GetAnimetor()->GetAnimationClip()->ReservationAnimation("slash", "attack2", 0.5f, raito_);
	}*/
	ImGui::DragFloat("raito", &raito_, 0.01f);
}
#endif
