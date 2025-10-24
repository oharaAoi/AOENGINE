#include "TitleScene.h"
#include "Engine.h"
#include "Engine/System/Input/Input.h"
#include "Engine/Lib/Json/JsonItems.h"

TitleScene::TitleScene() {}
TitleScene::~TitleScene() {Finalize();}

void TitleScene::Finalize() {}

void TitleScene::Init() {

	Engine::GetCanvas2d()->Init();

	JsonItems* adjust = JsonItems::GetInstance();
	adjust->Init("GameScene");

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
	
	titleUIs_ = std::make_unique<TitleUIs>();
	titleUIs_->Init();

	fadePanel_ = std::make_unique<FadePanel>();
	fadePanel_->Init();
	fadePanel_->SetBlackOutOpen();

	gameModeGuide_ = std::make_unique<GameModeGuide>();
	gameModeGuide_->Init();

	DirectionalLight* light = Render::GetLightGroup()->GetDirectionalLight();
	light->SetIntensity(0.3f);

	putButton_ = false;
}

void TitleScene::Update() {
	
	titleUIs_->Update();

	fadePanel_->Update();

	if (putButton_) {
		if (fadePanel_->GetIsFinished()) {
			gameModeGuide_->Open();
		}
	} else {
		if (Input::GetInstance()->IsTriggerButton(XInputButtons::BUTTON_A) || Input::GetInstance()->GetKey(DIK_SPACE)) {
			fadePanel_->SetBlackOut();
			putButton_ = true;
		}
	}

	gameModeGuide_->Update();

	if (gameModeGuide_->Decide()) {
		if (gameModeGuide_->GetSelectModeType() == SelectModeType::ToGame) {
			nextSceneType_ = SceneType::GAME;
		} else if (gameModeGuide_->GetSelectModeType() == SelectModeType::ToTutorial) {
			nextSceneType_ = SceneType::TUTORIAL;
		}
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
