#include "TitleScene.h"
#include "Engine.h"
#include "Engine/System/Input/Input.h"
#include "Engine/Lib/Json/JsonItems.h"

TitleScene::TitleScene() {
}

TitleScene::~TitleScene() {
	Finalize();
}

void TitleScene::Finalize() {
	sceneRenderer_->Finalize();
}

void TitleScene::Init() {
#ifdef _DEBUG
	EditorWindows::GetInstance()->Reset();
#endif

	Engine::GetCanvas2d()->Init();

	JsonItems* adjust = JsonItems::GetInstance();
	adjust->Init("GameScene");

	sceneRenderer_ = SceneRenderer::GetInstance();
	sceneRenderer_->Init();
	EditorWindows::GetInstance()->SetSceneRenderer(sceneRenderer_);

	// -------------------------------------------------
	// ↓ cameraの初期化
	// -------------------------------------------------
	camera2d_ = std::make_unique<Camera2d>();
	camera3d_ = std::make_unique<Camera3d>();
	debugCamera_ = std::make_unique<DebugCamera>();

	camera2d_->Init();
	camera3d_->Init();
	debugCamera_->Init();
	
	skybox_ = SceneRenderer::GetInstance()->AddObject<Skybox>("Skybox", "Object_Skybox.json", -999);

	//testObject_ = std::make_unique<TestObject>();
	//testObject_->Init();

	titleUIs_ = std::make_unique<TitleUIs>();
	titleUIs_->Init();

	fadePanel_ = std::make_unique<FadePanel>();
	fadePanel_->Init();
	fadePanel_->SetBlackOutOpen();

	DirectionalLight* light = Render::GetLightGroup()->GetDirectionalLight();
	light->SetIntensity(0.3f);

	putButton_ = false;
}

void TitleScene::Update() {
	
	titleUIs_->Update();

	fadePanel_->Update();

	if (putButton_) {
		if (fadePanel_->GetIsFinished()) {
			nextSceneType_ = SceneType::GAME;
		}
	}

	// -------------------------------------------------
	// ↓ 入力処理
	// -------------------------------------------------

	if (Input::GetInstance()->IsPressButton(XInputButtons::BUTTON_A) || Input::GetInstance()->GetKey(DIK_SPACE)) {
		fadePanel_->SetBlackOut();
		putButton_ = true;
	} 

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

void TitleScene::Draw() const {
	
	sceneRenderer_->Draw();
}
