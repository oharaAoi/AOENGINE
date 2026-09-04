#include "TestScene.h"
#include "Engine/Render/Render.h"
#include "Engine/Module/Components/GameObject/BaseGameObject.h"
#include "Engine/System/Manager/PrefabManager.h"
#include "Engine/Utilities/SceneObjectFinder.h"

/// game
#include "Game/Actor/Player/Player.h"
#include "Game/WorldObject/Block.h"

TestScene::TestScene() {}
TestScene::~TestScene() { Finalize(); }

void TestScene::Finalize()
{
	// ステージのブロックを SceneWorld から破棄し、連結グループ表を空にする。
	// TestScene のデストラクタからも呼ばれるため、複数回呼ばれても安全であること。
	ClearStage();
	player_.reset();
}

void TestScene::Init()
{
	AOENGINE::Render::GetLightGroup()->Load();

	player_ = std::make_unique<Player>();

	// 着地したブロックのグループをPlayerへ渡すコールバックを衝突ペアへ登録する
	playerBlockCallBacks_.SetPlayer(player_.get());
	playerBlockCallBacks_.Init();
	playerBlockCallBacks_.SetPair(collisionManager_.get(), "Player", "Block");
	stageBlockField_.SetBlockCollisionCallBacks(&playerBlockCallBacks_);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// 更新
//////////////////////////////////////////////////////////////////////////////////////////////////
void TestScene::Update()
{
	if (player_) {
		player_->Update();
	}

#ifdef _DEVELOPMENT
	// 調整パラメータの編集 + Save/Load
	if (player_) {
		ImGui::Begin("Player");
		player_->Debug_Gui();
		ImGui::End();
	}
#endif
}

void TestScene::OnPlayStart()
{

	// Player初期化
	player_->Init(ResolvePlayerBody());
	// 接続したグループを集合・打ち上げさせるために連結グループ表を渡す
	player_->SetBlockField(&stageBlockField_);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ステージの生成・片付け
//////////////////////////////////////////////////////////////////////////////////////////////////

void TestScene::SetupStage()
{
	stageSegment_.LoadBlockData("./Project/Assets/Game/StageData/test.csv");
	stageBlockField_.BuildSegment(stageSegment_, 0);
}

void TestScene::ClearStage()
{
	// ブロックの実体を破棄するため、先に参照しているものを手放させる
	if (player_) {
		player_->ResetStageReferences();
	}
	playerBlockCallBacks_.ClearBlocks();
	stageBlockField_.Clear();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// Playerの本体の用意
//////////////////////////////////////////////////////////////////////////////////////////////////

AOENGINE::BaseGameObject* TestScene::ResolvePlayerBody()
{
	// Sceneに置かれているPlayerを優先して使う
	if (AOENGINE::BaseGameObject* body = FindSceneObject<AOENGINE::BaseGameObject>("Player")) {
		return body;
	}

	// 無ければPrefabから生成する
	AOENGINE::SceneObject* root = AOENGINE::PrefabManager::GetInstance()->Instantiate("Player");
	return dynamic_cast<AOENGINE::BaseGameObject*>(root);
}
