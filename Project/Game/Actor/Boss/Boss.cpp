#include "Boss.h"

#include "Engine/Module/Components/GameObject/BaseGameObject.h"
#include "Engine/Module/Components/WorldTransform.h"
#include "Engine/System/Manager/ImGuiManager.h"
#include"Engine/Lib/GameTimer.h"

using namespace AOENGINE;

///////////////////////////////////////////////////////////////////////////////////////////////
//  初期化
///////////////////////////////////////////////////////////////////////////////////////////////

void Boss::Init(BaseGameObject* body) {
	Bind(body);

	// 保存済みの調整値を読み込む
	parameter_.Load();
	currentHp_ = parameter_.hp;

	if (WorldTransform* transform = GetTransform()) {
		position_ = transform->GetTranslate();
	}

	// 最初の行動をセットする
	behaviorController_.Init(*this);
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  更新
///////////////////////////////////////////////////////////////////////////////////////////////

void Boss::Update(const Math::Matrix4x4& viewProjection) {
	if (!IsValid()) {
		return;
	}

	// スクリーン座標上の固定位置に見えるワールド座標を求める
	const ScreenWorldPlaneAnchor::Params anchorParams{ parameter_.screenPos, parameter_.worldZ };
	position_ = screenAnchor_.Solve(viewProjection, anchorParams);

	if (WorldTransform* transform = GetTransform()) {
		transform->SetTranslate(position_);
	}

	// 基準位置を反映した後に行動を進める
	behaviorController_.Update(*this, AOENGINE::GameTimer::DeltaTime());
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  ダメージ
///////////////////////////////////////////////////////////////////////////////////////////////

void Boss::Damage(float amount) {
	currentHp_ -= amount;
	if (currentHp_ < 0.0f) {
		currentHp_ = 0.0f;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  デバッグ表示
///////////////////////////////////////////////////////////////////////////////////////////////

void Boss::Debug_Gui() {
	const char* bodyState = "null";
	if (IsValid()) {
		bodyState = "resolved";
	}

	ImGui::Text("body: %s", bodyState);
	ImGui::Text("hp: %.1f / %.1f", currentHp_, parameter_.hp);
	ImGui::DragFloat3("world position", &position_.x, 0.1f);

	// ボスの状態を可視化 + 行動の強制切り替え
	behaviorController_.Debug_Gui(*this);
}
