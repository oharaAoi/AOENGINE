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
	EditorWindows::GetInstance()->Reset();

	JsonItems* adjust = JsonItems::GetInstance();
	adjust->Init("GameScene");

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

	titleUIs_ = std::make_unique<TitleUIs>();
	titleUIs_->Init();

	fadePanel_ = std::make_unique<FadePanel>();
	fadePanel_->Init();
	fadePanel_->SetBlackOutOpen();

	putButton_ = false;
}

void TitleScene::Update() {
	skybox_->Update();

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

	if (Input::GetInstance()->GetPressPadTrigger(XInputButtons::BUTTON_A) || Input::GetInstance()->GetKey(DIK_SPACE)) {
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
}

void TitleScene::Draw() const {
	skybox_->Draw();

	Engine::SetPipeline(PSOType::Sprite, "Sprite_Normal.json");
	titleUIs_->Draw();
	fadePanel_->Draw();
}
