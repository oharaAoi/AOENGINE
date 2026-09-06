#include "PlayerUI.h"
#include "Engine/Utilities/SceneObjectFinder.h"
#include "Engine/Utilities/Logger.h"

//////////////////////////////////////////////////////////////////////////////////////////////////
// 初期化処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void PlayerUI::Init() {
	hp_ = FindSceneObject<AOENGINE::Sprite>("PlayerHP");

	// UIを置いていないSceneからも同じ手順で初期化されるので、
	if (!hp_) {
		AOENGINE::Logger::CommentLog("PlayerHPがSceneに無いため、HPゲージは出しません");
		return;
	}

	hp_->SetFillMethod(FillMethod::Vertical);
	hp_->SetFillStartingPoint(FillStartingPoint::Bottom);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// 更新処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void PlayerUI::Update(Player* player) {
	if (hp_ == nullptr || player == nullptr) {
		return;
	}

	// 最大HPが0だと割合が出せないので、その場合は満タン扱いにする
	const float maxHp = player->GetMaxHp();
	if (maxHp <= 0.0f) {
		hp_->FillAmount(0.0f);
		return;
	}

	const float raito = player->GetCurrentHp() / maxHp;
	hp_->FillAmount(1.0f - raito);
}
