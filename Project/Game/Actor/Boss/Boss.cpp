#include "Boss.h"

#include "Engine/Module/Components/GameObject/BaseGameObject.h"
#include "Engine/Module/Components/WorldTransform.h"
#include "Engine/System/Manager/ImGuiManager.h"

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

	// 当たり判定 
	collision_.CreateCollider(GetGameObject(), parameter_.hitSize);
	// コールバックとしてダメージ関数呼ぶのをセット
	collision_.SetOnHit([this]() { Damage(1.0f); });
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  更新
///////////////////////////////////////////////////////////////////////////////////////////////

void Boss::Update(const Math::Matrix4x4& viewProjection) {
	if (!IsValid()) {
		return;
	}

	// スクリーン座標上の固定位置に見えるワールド座標を求める
	const BossScreenAnchor::Params anchorParams{ parameter_.screenPos, parameter_.worldZ };
	position_ = screenAnchor_.Solve(viewProjection, anchorParams);

	if (WorldTransform* transform = GetTransform()) {
		transform->SetTranslate(position_);
	}
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
}
