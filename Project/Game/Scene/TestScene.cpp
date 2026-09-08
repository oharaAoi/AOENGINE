#include "TestScene.h"
#include "Engine/Render/Render.h"
#include "Engine/Module/Components/GameObject/BaseGameObject.h"
#include "Engine/System/Manager/PrefabManager.h"
#include "Engine/Utilities/SceneObjectFinder.h"
#include "Engine/Module/Components/Effect/BaseParticles.h"
#include "Engine/Module/Components/3d/WorldTextComponent.h"
#include "Engine/System/Manager/ParticleEffectManager.h"

/// game
#include "Game/Actor/Player/Player.h"
#include "Game/WorldObject/Block.h"
#include <System/Manager/ParticleManager.h>

TestScene::TestScene() {}
TestScene::~TestScene() { Finalize(); }

void TestScene::Finalize()
{
	// ステージのブロックを SceneWorld から破棄し、連結グループ表を空にする。
	// TestScene のデストラクタからも呼ばれるため、複数回呼ばれても安全であること。
	ClearStage();
	
}

void TestScene::Init()
{
	AOENGINE::Render::GetLightGroup()->Load();

	player_ = std::make_unique<Player>();

		
	
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// 更新
//////////////////////////////////////////////////////////////////////////////////////////////////
void TestScene::Update()
{

}

void TestScene::OnPlayStart()
{
	auto effect =
		AOENGINE::ParticleEffectManager::GetInstance()->Play(
			"PlayerHitEffect"
		);

	/*AOENGINE::SceneObject* obj = AOENGINE::PrefabManager::GetInstance()->Instantiate("test3d");
	AOENGINE::BaseGameObject* gameObject =
		dynamic_cast<AOENGINE::BaseGameObject*>(obj);
	AOENGINE::WorldTextComponent* text = gameObject->GetWorldTextComponent();
	if (text) {
		text->SetText("seisei dekimasita");
	}*/
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
