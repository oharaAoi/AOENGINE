#include "TitleScene.h"

#include "Engine/Core/Engine.h"

#include "Engine/Lib/GameTimer.h"
#include "Engine/Module/Components/GameObject/BaseGameObject.h"
#include "Engine/Module/Components/WorldTransform.h"
#include "Engine/System/Input/Input.h"
#include "Engine/Utilities/SceneObjectFinder.h"
#include "Engine/System/Manager/ImGuiManager.h"

TitleScene::~TitleScene() {
}

void TitleScene::Finalize() {
	Engine::GetSoundManager()->Stop(bgmHandle_);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// 初期化処理(インスタンスの宣言など)
//////////////////////////////////////////////////////////////////////////////////////////////////

void TitleScene::Init() {
	bgmHandle_ = Engine::GetSoundManager()->Play("TitleBGM");
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// スタート時に呼ばれる処理(パラメータの読み込み、設定などはココで行う)
//////////////////////////////////////////////////////////////////////////////////////////////////

void TitleScene::OnPlayStart() {
	titleUI_.Init();
	meteoEffect_.Init();
	earthParameter_.Load();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// 更新
//////////////////////////////////////////////////////////////////////////////////////////////////

void TitleScene::Update() {

	// 背景の隕石を流す
	meteoEffect_.Update(AOENGINE::GameTimer::DeltaTime());

	// 地球を画面上の決まった位置へ置き直す
	UpdateEarth();

#ifdef _DEVELOPMENT
	ImGui::Begin("TitleScene Parameters");
	if (ImGui::CollapsingHeader("Meteo")) {
		ImGui::PushID("TitleMeteo");
		meteoEffect_.Debug_Gui();
		ImGui::PopID();
	}
	if (ImGui::CollapsingHeader("Earth")) {
		ImGui::PushID("TitleEarth");
		earthParameter_.Debug_Gui();
		earthParameter_.SaveAndLoad();
		ImGui::PopID();
	}
	ImGui::End();
#endif

	// 次のシーンの選択
	TitleUI::TitleItem current = titleUI_.Update();
	if (current == TitleUI::TitleItem::Start) {
		nextSceneType_ = SceneType::Game;
	} else if (current == TitleUI::TitleItem::Tutorial) {
		nextSceneType_ = SceneType::Tutorial;
	} else if (current == TitleUI::TitleItem::Exit) {
		endRequest_ = true;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// 背景の地球
//////////////////////////////////////////////////////////////////////////////////////////////////

void TitleScene::UpdateEarth() {

	AOENGINE::BaseGameObject* earth = FindSceneObject<AOENGINE::BaseGameObject>(kEarthName);
	if (earth == nullptr) {
		return;
	}

	AOENGINE::WorldTransform* transform = earth->GetTransform();
	if (transform == nullptr) {
		return;
	}

	// ボスと同じやり方。画面上のピクセル位置から、その奥行きのワールド座標を引く。
	// ワールド座標を直接置くのと違い、解像度やカメラが変わっても画面上の位置は変わらない
	const Math::Matrix4x4 viewProjection =
		camera3d_->GetViewMatrix() * camera3d_->GetProjectionMatrix();
	const ScreenWorldPlaneAnchor::Params params{ earthParameter_.screenPos, earthParameter_.worldZ };

	transform->SetTranslate(earthAnchor_.Solve(viewProjection, params));

	const float scale = earthParameter_.scale;
	transform->SetScale(Math::Vector3(scale, scale, scale));
}