#include "BossStateStan.h"
#include "Game/Actor/Boss/Boss.h"
#include "Engine/Lib/Json/JsonItems.h"
#include "Game/Actor/Boss/State/BossStateNormal.h"
#include "Game/Actor/Boss/Action/BossActionIdle.h"
#include "Game/UI/Boss/BossUIs.h"

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossStateStan::OnStart() {
	SetName("StanState");

	stateTime_ = 0.0f;
	param_.FromJson(JsonItems::GetData(stateMachine_->GetName(), param_.GetName()));

	pOwner_->GetGameObject()->GetRigidbody()->SetGravity(true);

	pOwner_->SetIsStan(true);
	pOwner_->GetUIs()->PopStan();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossStateStan::OnUpdate() {
	stateTime_ += GameTimer::DeltaTime();

	if (stateTime_ >= param_.stanTime) {
		stateMachine_->ChangeState<BossStateNormal>();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 終了処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossStateStan::OnExit() {
	pOwner_->GetGameObject()->GetRigidbody()->SetGravity(false);

	pOwner_->ResetStan();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集
///////////////////////////////////////////////////////////////////////////////////////////////

void BossStateStan::Debug_Gui() {
	ImGui::DragFloat("stanTime", &param_.stanTime, 0.1f);
	if (ImGui::Button("Save")) {
		JsonItems::Save(stateMachine_->GetName(), param_.ToJson(param_.GetName()));
	}
	if (ImGui::Button("Apply")) {
		param_.FromJson(JsonItems::GetData(stateMachine_->GetName(), param_.GetName()));
	}
}
