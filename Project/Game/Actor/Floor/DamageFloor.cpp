#include "DamageFloor.h"

#include "Engine/Lib/GameTimer.h"
#include "Engine/Lib/Math/MyMath.h"
#include "Engine/Module/Components/Collider/BoxCollider.h"
#include "Engine/Module/Components/GameObject/BaseGameObject.h"
#include "Engine/Module/Components/WorldTransform.h"
#include "Engine/WinApp/WinApp.h"

void DamageFloor::Init(AOENGINE::BaseGameObject* body) {
	Bind(body);
	parameter_.Load();

	// コライダーセット
	if (!body) { return; }
	AOENGINE::BaseCollider* collider = body->GetCollider("DamageFloor");
	if (!collider) {
		collider = body->SetCollider("DamageFloor", ColliderShape::AABB);
	}
	// boxサイズ決定
	if (auto* box = dynamic_cast<AOENGINE::BoxCollider*>(collider)) {
		box->SetSize(parameter_.hitSize);
	}

	if (collider) {
		collider->SetIsTrigger(true);

		// Block / StepBlock は Player より後に生成されるため、床側のマスクにも相手を入れておかないと
		// CollisionManager の片方向マスク判定で拾えないことがある
		collider->SetTarget("Block");
		collider->SetTarget("StepBlock");
	}

	// 初期パラメータ
	followVelocity_ = CVector3::ZERO;
	hasFollowStarted_ = false;

	if (AOENGINE::WorldTransform* transform = GetTransform()) {
		position_ = transform->GetTranslate();
	}
}

void DamageFloor::Update(const Math::Matrix4x4& viewProjection) {
	if (!IsValid()) { return; }

	// 0〜1で持っている画面上の置き場所を、今の解像度のピクセル座標へ直す
	const Math::Vector2 screenPosition{
		parameter_.viewportAnchor.x * static_cast<float>(AOENGINE::WinApp::sClientWidth),
		parameter_.viewportAnchor.y * static_cast<float>(AOENGINE::WinApp::sClientHeight)
	};

	// そのピクセル座標に見える、奥行き worldZ の平面上のワールド座標を求める
	const Math::Vector3 anchorPosition =
		screenAnchor_.Solve(viewProjection, { screenPosition, parameter_.worldZ });

	if (!hasFollowStarted_) {
		// 1フレーム目から遅らせると、原点から飛んでくることになる
		position_ = anchorPosition;
		followVelocity_ = CVector3::ZERO;
		hasFollowStarted_ = true;
	} else {
		// カメラが動いた分に遅れて付いていく
		position_ = SmoothDamp(
			position_, anchorPosition, followVelocity_,
			parameter_.followSmoothTime, parameter_.followMaxSpeed,
			AOENGINE::GameTimer::DeltaTime());
	}

	if (AOENGINE::WorldTransform* transform = GetTransform()) {
		transform->SetTranslate(position_);
	}
}
