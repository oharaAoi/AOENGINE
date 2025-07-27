#include "TestScene.h"
#include "Engine.h"
#include "Engine/System/Input/Input.h"
#include "Engine/Lib/Json/JsonItems.h"
#include "Engine/System/Editer/Window/EditorWindows.h"
#include "Engine/Module/Geometry/Polygon/PlaneGeometry.h"

TestScene::TestScene() {}
TestScene::~TestScene() { Finalize(); }

void TestScene::Finalize() {
	sceneRenderer_->Finalize();
}

void TestScene::Init() {
	EditorWindows::GetInstance()->Reset();

	JsonItems* adjust = JsonItems::GetInstance();
	adjust->Init("TestScene");

	sceneRenderer_ = SceneRenderer::GetInstance();
	sceneRenderer_->Init();

	// -------------------------------------------------
	// ↓ cameraの初期化
	// -------------------------------------------------
	camera2d_ = std::make_unique<Camera2d>();
	camera3d_ = std::make_unique<Camera3d>();
	debugCamera_ = std::make_unique<DebugCamera>();

	camera2d_->Init();
	camera3d_->Init();
	debugCamera_->Init();

	skybox_ = std::make_unique<Skybox>();
	skybox_->Init();
	Render::SetSkyboxTexture(skybox_->GetTexture());

	testObject_ = std::make_unique<TestObject>();
	testObject_->Init();

	EditorWindows::AddObjectWindow(Render::GetLightGroup(), "LightGroup");
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// 更新
//////////////////////////////////////////////////////////////////////////////////////////////////
void TestScene::Update() {
	skybox_->Update();
	testObject_->Update();

	// -------------------------------------------------
	// ↓ cameraの更新 
	// -------------------------------------------------
	if (debugCamera_->GetIsActive()) {
		debugCamera_->Update();
	} else {
		camera3d_->Update();
	}
	camera2d_->Update();

	sceneRenderer_->Update();
}

void TestScene::Draw() const {
	skybox_->Draw();
	// Sceneの描画
	sceneRenderer_->Draw();
}
