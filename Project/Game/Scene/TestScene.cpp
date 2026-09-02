#include "TestScene.h"
#include "Engine/Render/Render.h"

/// Game
#include "Game/Stage/StageSegment.h"

TestScene::TestScene() {}
TestScene::~TestScene() { Finalize(); }

void TestScene::Finalize() {}

void TestScene::Init() {
	BaseScene::Init();

	AOENGINE::Render::GetLightGroup()->Load();

	StageSegment seg;
	seg.LoadBlockData("./Project/Assets/Game/StageData/test.csv");
	seg.SetupSegmentOnWorld();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// 更新
//////////////////////////////////////////////////////////////////////////////////////////////////
void TestScene::Update() {

}
