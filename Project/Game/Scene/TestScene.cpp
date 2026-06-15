#include "TestScene.h"
#include "Engine/Core/Engine.h"
#include "Engine/Render/Render.h"
#include "Engine/Lib/Json/JsonItems.h"
#include "Engine/System/Editor/Window/EditorWindows.h"

TestScene::TestScene() {}
TestScene::~TestScene() { Finalize(); }

void TestScene::Finalize() {}

void TestScene::Init() {
	Engine::GetCanvas2d()->Init();
	
	AOENGINE::JsonItems* adjust = AOENGINE::JsonItems::GetInstance();
	adjust->Init();

	collisionManager_ = std::make_unique<AOENGINE::CollisionManager>();
	collisionManager_->Init();

	pSceneRenderer_ = AOENGINE::SceneRenderer::GetInstance();
	pSceneRenderer_->Init();

	skybox_ = AOENGINE::SceneRenderer::GetInstance()->AddObject<Skybox>("Skybox", "Object_Skybox.json", -999);
	AOENGINE::Render::SetSkyboxTexture(skybox_->GetTexture());

	AOENGINE::Render::GetLightGroup()->Load();

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
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// 更新
//////////////////////////////////////////////////////////////////////////////////////////////////
void TestScene::Update() {
	
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
