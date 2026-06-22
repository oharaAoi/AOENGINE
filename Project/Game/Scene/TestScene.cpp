#include "TestScene.h"
#include "Engine/Core/Engine.h"
#include "Engine/Render/Render.h"
#include "Engine/Lib/Json/JsonItems.h"
#include "Engine/System/Editor/Window/EditorWindows.h"

TestScene::TestScene() {}
TestScene::~TestScene() { Finalize(); }

void TestScene::Finalize() {}

void TestScene::Init() {
	BaseScene::Init();
	Engine::GetCanvas2d()->Init();

	AOENGINE::JsonItems* adjust = AOENGINE::JsonItems::GetInstance();
	adjust->Init();

	skybox_ = AOENGINE::SceneRenderer::GetInstance()->AddObject<Skybox>("Skybox", "Object_Skybox.json", -999);
	AOENGINE::Render::SetSkyboxTexture(skybox_->GetTexture());

	AOENGINE::Render::GetLightGroup()->Load();

}

//////////////////////////////////////////////////////////////////////////////////////////////////
// 更新
//////////////////////////////////////////////////////////////////////////////////////////////////
void TestScene::Update() {

}
