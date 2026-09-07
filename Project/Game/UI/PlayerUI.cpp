#include "PlayerUI.h"
#include "Engine/Utilities/SceneObjectFinder.h"
#include "Engine/Utilities/Logger.h"

//////////////////////////////////////////////////////////////////////////////////////////////////
// 初期化処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void PlayerUI::Init(){
	hp_ = FindSceneObject<AOENGINE::Sprite>("PlayerHP");
	connectableTime_ = FindSceneObject<AOENGINE::Sprite>("LauncherGauge");

	// UIを置いていないSceneからも同じ手順で初期化されるので、
	if(!hp_){
		AOENGINE::Logger::CommentLog("PlayerHPがSceneに無いため、HPゲージは出しません");
		return;
	}

	if(!connectableTime_){
		AOENGINE::Logger::CommentLog("ConnectableTimeがSceneに無いため、接続可能時間は出しません");
		return;
	}

	hp_->SetFillMethod(FillMethod::Vertical);
	hp_->SetFillStartingPoint(FillStartingPoint::Bottom);

	connectableTime_->SetFillMethod(FillMethod::Vertical);
	connectableTime_->SetFillStartingPoint(FillStartingPoint::Bottom);

}

//////////////////////////////////////////////////////////////////////////////////////////////////
// 更新処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void PlayerUI::Update(Player* player){
	if(player == nullptr){
		return;
	}

	UpdateHP(player);
	UpdateConnectableTime(player);
}

void PlayerUI::UpdateHP(Player* player){
	if(hp_ == nullptr){
		return;
	}

	// 最大HPが0だと割合が出せないので、その場合は満タン扱いにする
	const float maxHp = player->GetMaxHp();
	if(maxHp <= 0.0f){
		hp_->FillAmount(0.0f);
		return;
	}

	const float raito = player->GetCurrentHp() / maxHp;
	hp_->FillAmount(1.0f - raito);
}

void PlayerUI::UpdateConnectableTime(Player* player){
	if(connectableTime_ == nullptr){
		return;
	}

	const auto* blockGroupConnectState = player->GetBlockGroupConnectState();
	if(blockGroupConnectState == nullptr){
		return;
	}

	const float connectableTime = blockGroupConnectState->GetParams().connectableTime;
	const float remainingTime = blockGroupConnectState->GetRemainingTime();

	if(connectableTime <= 0.f){
		connectableTime_->FillAmount(0.0f);
		return;
	}

	const float raito = remainingTime / connectableTime;
	connectableTime_->FillAmount(1.0f - raito);
}
