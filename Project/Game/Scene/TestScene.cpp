#include "TestScene.h"
#include "Engine.h"
#include "Engine/Lib/Json/JsonItems.h"
#include "Engine/System/Editer/Window/EditorWindows.h"

TestScene::TestScene() {}
TestScene::~TestScene() { Finalize(); }

void TestScene::Finalize() {}

void TestScene::Init() {
	Engine::GetCanvas2d()->Init();
	
	AOENGINE::JsonItems* adjust = AOENGINE::JsonItems::GetInstance();
	adjust->Init("TestScene");

	AOENGINE::Render::GetLightGroup()->Load();

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

	jet_ = std::make_unique<JetEngine>();
	jet_->Init();

	attackArmor_ = std::make_unique<AttackArmor>();
	attackArmor_->Init();

	/*AOENGINE::BaseGameObject *object_ = AOENGINE::SceneRenderer::GetInstance()->AddObject<AOENGINE::BaseGameObject>("ground", "Object_PBR.json");
	object_->SetObject("floor.obj", MaterialType::PBR);*/

	/*AOENGINE::BaseGameObject *object_ = AOENGINE::SceneRenderer::GetInstance()->AddObject<AOENGINE::BaseGameObject>("ground", "Object_PBR.json");
	object_->SetObject("floor.obj", MaterialType::PBR);*/

	AOENGINE::EditorWindows::AddObjectWindow(jet_.get(), "jet");
	AOENGINE::EditorWindows::AddObjectWindow(attackArmor_.get(), "attackArmor");
	//AOENGINE::EditorWindows::AddObjectWindow(object_, "ground");
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// 更新
//////////////////////////////////////////////////////////////////////////////////////////////////
void TestScene::Update() {
	jet_->Update(0.0f);
	attackArmor_->Update();

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
