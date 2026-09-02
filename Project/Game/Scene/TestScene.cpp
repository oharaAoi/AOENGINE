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
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// 更新
//////////////////////////////////////////////////////////////////////////////////////////////////
void TestScene::Update() {

}

void TestScene::OnPlayStart(){
	StageSegment seg;
	seg.LoadBlockData("./Project/Assets/Game/StageData/test.csv");
	seg.SetupSegmentOnWorld();
}
