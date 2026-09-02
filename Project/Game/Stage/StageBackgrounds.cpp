#include "StageBackgrounds.h"

// engine
#include "Engine/Render/SceneRenderer.h"
#include "Engine/System/Manager/PrefabManager.h"
#include "Engine/Module/Components/GameObject/BaseGameObject.h"

// game
#include "Game/Stage/StageSegment.h"

//////////////////////////////////////////////////////////////////////////////////////////////////
// 初期化処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void StageBackgrounds::Init() {
	parameter_.Load();

	currentIndex_ = -1;

	for (size_t i = 0; i < 2; ++i) {
		CreateBackground();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// 更新処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void StageBackgrounds::Update(const Math::Vector3& playerPos) {
	BackgroundLoop(playerPos);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// 背景をループさせる
//////////////////////////////////////////////////////////////////////////////////////////////////

void StageBackgrounds::BackgroundLoop(const Math::Vector3& playerPos) {
	float playerY = playerPos.y;

	// playerのY座標がindexの値 x スクロールの高さ超えたら次を生成
	if (playerY >= currentIndex_ * parameter_.scrollHeight) {
		CreateBackground();

		// 前の背景の削除
		AOENGINE::ObjectHandle frontHandle = objHandles_.front();
		AOENGINE::SceneRenderer::GetInstance()->DestroyObject(frontHandle);
		objHandles_.pop();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// 背景の生成
//////////////////////////////////////////////////////////////////////////////////////////////////

void StageBackgrounds::CreateBackground() {
	// 背景の追加生成
	AOENGINE::SceneObject* root =
		AOENGINE::PrefabManager::GetInstance()->Instantiate("Plane");

	AOENGINE::BaseGameObject* background = dynamic_cast<AOENGINE::BaseGameObject*>(root);

	if (!background) { return; }

	objHandles_.push(root->GetHandle());
	background->GetTransform()->SetTranslationY((currentIndex_ + 1) * parameter_.scrollHeight);
	currentIndex_++;

	StageSegment seg;
	seg.LoadBlockData("./Project/Assets/Game/StageData/test.csv");
	seg.SetupSegmentOnWorld(parameter_.scrollHeight * currentIndex_);
}
