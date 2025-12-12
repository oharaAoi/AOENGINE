#include "TestScene.h"
#include "Engine.h"
#include "Engine/Lib/Json/JsonItems.h"
#include "Engine/System/Editer/Window/EditorWindows.h"

TestScene::TestScene() {}
TestScene::~TestScene() { Finalize(); }

void TestScene::Finalize() {}

void TestScene::Init() {
	Engine::GetCanvas2d()->Init();
	AOENGINE::EditorWindows::GetInstance()->Reset();

	AOENGINE::JsonItems* adjust = AOENGINE::JsonItems::GetInstance();
	adjust->Init("TestScene");

	LoadScene("./Project/Packages/Game/Assets/Scene/", "template", ".json");

	// -------------------------------------------------
	// ↓ cameraの初期化
	// -------------------------------------------------
	camera2d_ = std::make_unique<Camera2d>();
	camera3d_ = std::make_unique<Camera3d>();
	debugCamera_ = std::make_unique<DebugCamera>();

	camera2d_->Init();
	camera3d_->Init();
	debugCamera_->Init();
	debugCamera_->SetIsActive(true);

	laser_ = std::make_unique<LaserBullet>();
	laser_->Init();

	jet_ = std::make_unique<JetEngineBurn>();
	jet_->Init();

	AOENGINE::EditorWindows::AddObjectWindow(laser_.get(), "Laser");
	AOENGINE::EditorWindows::AddObjectWindow(jet_.get(), "jet");
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// 更新
//////////////////////////////////////////////////////////////////////////////////////////////////
void TestScene::Update() {

	laser_->Update();
	jet_->Update();

	// -------------------------------------------------
	// ↓ cameraの更新 
	// -------------------------------------------------
	if (debugCamera_->GetIsActive()) {
		debugCamera_->Update();
	} else {
		camera3d_->Update();
	}
	camera2d_->Update();


}
