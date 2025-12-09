#include "JetEngineBurn.h"
#include "Engine.h"
#include "Engine/Lib/GameTimer.h"
#include <Render/SceneRenderer.h>

JetEngineBurn::~JetEngineBurn() {
	AOENGINE::SceneRenderer::GetInstance()->ReleaseObject(object_);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void JetEngineBurn::Init() {
	param_.Load();
	
	object_ = AOENGINE::SceneRenderer::GetInstance()->AddObject<AOENGINE::BaseGameObject>("JetBurn", "Object_Add.json", 100);
	object_->SetObject("sphere.obj");
	object_->SetTexture("laser.png");
	object_->SetIsLighting(false);
	object_->SetEnableShadow(false);
	object_->SetColor(param_.color);

	transform_ = object_->GetTransform();
	transform_->SetScale(param_.scale);
	transform_->SetRotate(param_.rotate);
	transform_->SetTranslate(param_.translate);

	shaderGraph_ = std::make_unique<AOENGINE::ShaderGraph>();
	shaderGraph_->Init("jetBurn");
	shaderGraph_->Load(param_.shaderGraphPath);

	for (auto& material : object_->GetMaterials()) {
		material.second->SetShaderGraph(shaderGraph_.get());
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void JetEngineBurn::Update() {
	shaderGraph_->Update();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集処理
///////////////////////////////////////////////////////////////////////////////////////////////

void JetEngineBurn::Debug_Gui() {
	object_->Debug_Gui();
	shaderGraph_->Debug_Gui();
	param_.shaderGraphPath = shaderGraph_->GetPath();
	param_.Debug_Gui();
}


void JetEngineBurn::Parameter::Debug_Gui() {
	ImGui::ColorEdit4("color", &color.r);
	ImGui::DragFloat3("scale", &scale.x, 0.1f);
	ImGui::DragFloat3("translate", &translate.x, 0.1f);
	SaveAndLoad();
}
