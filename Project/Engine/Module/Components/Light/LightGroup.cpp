#include "LightGroup.h"
#include "Engine/System/Editer/Window/EditorWindows.h"

using namespace AOENGINE;

LightGroup::LightGroup() {}
LightGroup::~LightGroup() {}

void LightGroup::Finalize() {
	directionalLight_->Finalize();
	pointLight_->Finalize();
	spotLight_->Finalize();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void LightGroup::Init(ID3D12Device* device) {
	directionalLight_ = std::make_unique<DirectionalLight>();
	pointLight_ = std::make_unique<PointLight>();
	spotLight_ = std::make_unique<SpotLight>();

	directionalLight_->Init(device, sizeof(DirectionalLight::DirectionalLightData));
	pointLight_->Init(device, sizeof(PointLight::PointLightData));
	spotLight_->Init(device, sizeof(SpotLight::SpotLightData));

	this->AddChild(directionalLight_.get());
	this->AddChild(pointLight_.get());
	this->AddChild(spotLight_.get());

	EditorWindows::AddObjectWindow(this, "LightGroup");
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void LightGroup::Update() {
	directionalLight_->SetEyePos(eyePos_);
	pointLight_->SetEyePos(eyePos_);
	spotLight_->SetEyePos(eyePos_);

	directionalLight_->Update();
	pointLight_->Update();
	spotLight_->Update();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ コマンドを積む
///////////////////////////////////////////////////////////////////////////////////////////////

void LightGroup::BindCommand(const Pipeline* pso, ID3D12GraphicsCommandList* commandList) {
	UINT index = pso->GetRootSignatureIndex("gDirectionalLight");
	directionalLight_->BindCommand(commandList, index);
	index = pso->GetRootSignatureIndex("gPointLight");
	pointLight_->BindCommand(commandList, index);
	index = pso->GetRootSignatureIndex("gSpotLight");
	spotLight_->BindCommand(commandList, index);
}

void LightGroup::Debug_Gui() {

}

void LightGroup::Load() {
	directionalLight_->LoadData();
	pointLight_->LoadData();
	spotLight_->LoadData();
}