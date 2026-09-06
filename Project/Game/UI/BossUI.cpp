#include "BossUI.h"
#include "Engine/Utilities/SceneObjectFinder.h"
#include "Engine/Utilities/Logger.h"

//////////////////////////////////////////////////////////////////////////////////////////////////
// 初期化処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void BossUI::Init() {
	hp_ = FindSceneObject<AOENGINE::Sprite>("BossHP");

	// ボスが出ないScene(チュートリアルなど)にはUIも置かれていないので、
	// 見つからない場合は出さないだけにして進行は止めない
	if (!hp_) {
		AOENGINE::Logger::CommentLog("BossHPがSceneに無いため、HPゲージは出しません");
		return;
	}

	hp_->SetFillMethod(FillMethod::Vertical);
	hp_->SetFillStartingPoint(FillStartingPoint::Bottom);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// 更新処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void BossUI::Update(Boss* boss) {
	if (hp_ == nullptr || boss == nullptr) {
		return;
	}

	// 最大HPが0だと割合が出せないので、その場合は満タン扱いにする
	const float maxHp = boss->GetMaxHp();
	if (maxHp <= 0.0f) {
		hp_->FillAmount(0.0f);
		return;
	}

	const float raito = boss->GetCurrentHp() / maxHp;
	hp_->FillAmount(1.0f - raito);
}
