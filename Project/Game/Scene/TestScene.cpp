#include "TestScene.h"
#include "Engine/Render/Render.h"

TestScene::TestScene() {}
TestScene::~TestScene() { Finalize(); }

void TestScene::Finalize()
{
	// ステージのブロックを SceneWorld から破棄し、連結グループ表を空にする。
	// TestScene のデストラクタからも呼ばれるため、複数回呼ばれても安全であること。
	stageSegment_.UnregisterFromWorld(&stageBlockField_);
	stageBlockField_.Clear();
}

void TestScene::Init()
{
	AOENGINE::Render::GetLightGroup()->Load();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// 更新
//////////////////////////////////////////////////////////////////////////////////////////////////
void TestScene::Update()
{
}

void TestScene::OnPlayStart()
{
	stageSegment_.LoadBlockData("./Project/Assets/Game/StageData/test.csv");
	stageSegment_.SetupSegmentOnWorld(&stageBlockField_, 0);
}
