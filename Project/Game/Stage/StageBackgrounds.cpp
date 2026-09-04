#include "StageBackgrounds.h"

// engine
#include "Engine/Render/SceneRenderer.h"
#include "Engine/System/Manager/PrefabManager.h"
#include "Engine/Module/Components/GameObject/BaseGameObject.h"

// game
#include "Game/Stage/StageSegment.h"
#include "Game/Stage/StageFactory.h"

namespace {
	/// 同時に存在させる背景の枚数
	constexpr size_t kKeepBackgroundCount = 2;
	/// 同時に存在させる段の数
	constexpr int kKeepSegmentCount = 2;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// 初期化処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void StageBackgrounds::Init(StageBlockField* field){
	parameter_.Load();

	backgroundIndex_ = -1;
	segmentIndex_ = 0;

	// 足元と次の分をあらかじめ用意しておく
	for(size_t i = 0; i < kKeepBackgroundCount; ++i){
		CreateBackground();
	}


	StageFactory::GetInstance().CreateStartSegment(field,segmentIndex_);
	for(int i = 1; i < kKeepSegmentCount; ++i){
		CreateSegment(field);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// 更新処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void StageBackgrounds::Update(StageBlockField* field,const Math::Vector3& playerPos){
	BackgroundLoop(playerPos);
	SegmentLoop(field,playerPos);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// 背景をループさせる
//////////////////////////////////////////////////////////////////////////////////////////////////

void StageBackgrounds::BackgroundLoop(const Math::Vector3& playerPos){
	// 間隔が未設定だと毎フレーム生成してしまうため、正の値のときだけループさせる
	if(parameter_.scrollHeight <= 0){
		return;
	}

	// playerのY座標が、最後に生成した背景の高さを超えたら次を生成する
	if(playerPos.y < static_cast<float>(backgroundIndex_ * parameter_.scrollHeight)){
		return;
	}

	CreateBackground();

	// 画面外に出た一番古い背景を削除する
	if(objHandles_.size() > kKeepBackgroundCount){
		AOENGINE::ObjectHandle frontHandle = objHandles_.front();
		AOENGINE::SceneRenderer::GetInstance()->DestroyObject(frontHandle);
		objHandles_.pop();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// 段をループさせる
//////////////////////////////////////////////////////////////////////////////////////////////////

void StageBackgrounds::SegmentLoop(StageBlockField* field,const Math::Vector3& playerPos){
	if(field == nullptr){
		return;
	}

	// playerのY座標が、最後に生成した段の高さを超えたら次の段を積む。
	// 背景と違い、段の高さはステージ実体の高さ(kBlockRow)そのものを使う
	if(playerPos.y < static_cast<float>(segmentIndex_ * kBlockRow)){
		return;
	}

	CreateSegment(field);

	// 画面外に出た下の段のブロックを破棄する
	field->DestroySegment(segmentIndex_ - kKeepSegmentCount);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// 背景の生成
//////////////////////////////////////////////////////////////////////////////////////////////////

void StageBackgrounds::CreateBackground(){
	// 背景の追加生成
	AOENGINE::SceneObject* root =
		AOENGINE::PrefabManager::GetInstance()->Instantiate("Plane");

	AOENGINE::BaseGameObject* background = dynamic_cast<AOENGINE::BaseGameObject*>(root);

	if(!background){ return; }

	++backgroundIndex_;

	objHandles_.push(root->GetHandle());
	background->GetTransform()->SetTranslationY(static_cast<float>(backgroundIndex_ * parameter_.scrollHeight));
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// 段の生成
//////////////////////////////////////////////////////////////////////////////////////////////////

void StageBackgrounds::CreateSegment(StageBlockField* field){
	if(field == nullptr){
		return;
	}

	++segmentIndex_;

	StageFactory::GetInstance().CreateFromRandom(field,segmentIndex_);
}
