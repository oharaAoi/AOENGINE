#include "TestObject.h"
#include "Engine/Render/SceneRenderer.h"

TestObject::TestObject() {
}

TestObject::~TestObject() {
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　
//////////////////////////////////////////////////////////////////////////////////////////////////

void TestObject::Init() {
	SetName("TestObject");
	object_ = SceneRenderer::GetInstance()->AddObject<BaseGameObject>("TestObject", "Object_Normal.json");
	transform_ = object_->GetTransform();
	object_->SetObject("player.gltf");
	object_->SetAnimater("./Game/Assets/Load/Models/", "player.gltf", true, true, false);

	EditorWindows::AddObjectWindow(this, "TestObject");

	swordMat_ = object_->GetAnimetor()->GetSkeleton()->GetSkeltonSpaceMat("mixamorig:RightHand") * transform_->GetWorldMatrix();

	sword_ = std::make_unique<Sword>();
	sword_->Init();
	sword_->GetTransform()->SetParent(swordMat_);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　
//////////////////////////////////////////////////////////////////////////////////////////////////

void TestObject::Update() {
	swordMat_ = object_->GetAnimetor()->GetSkeleton()->GetSkeltonSpaceMat("mixamorig:RightHand") * transform_->GetWorldMatrix();
}

void TestObject::Debug_Gui() {
	object_->Debug_Gui();
}