#include "BaseLight.h"

void BaseLight::Finalize() {
	lightBuffer_.Reset();
	cBuffer_.Reset();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BaseLight::Init(ID3D12Device* device, const size_t& size) {
	lightBuffer_ = CreateBufferResource(device, size);

	cBuffer_ = CreateBufferResource(device, sizeof(LightViewProjectionData));
	// データをマップ
	cBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&data_));
	lightPos_ = Vector3(0, 200, 0);
	direction_ = Vector3(0, -1, 0);
	direction_ = direction_.Normalize();

	baseParameter_.SetGroupName("Light");
	baseParameter_.SetName("baseParameter");
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BaseLight::Update() {
	baseParameter_.direction = baseParameter_.direction.Normalize();
	lightPos_ = baseParameter_.lightPos;
	direction_ = baseParameter_.direction;
	fovY_ = baseParameter_.fovY;
	near_ = baseParameter_.nearClip;
	far_ = baseParameter_.farClip;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 描画処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BaseLight::BindCommand(ID3D12GraphicsCommandList* commandList, const uint32_t& rootParameterIndex) {
	commandList->SetGraphicsRootConstantBufferView(rootParameterIndex, lightBuffer_->GetGPUVirtualAddress());
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ コマンドを積む
///////////////////////////////////////////////////////////////////////////////////////////////

void BaseLight::ViewBindCommand(ID3D12GraphicsCommandList* commandList, UINT index) const {
	commandList->SetGraphicsRootConstantBufferView(index, cBuffer_->GetGPUVirtualAddress());
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ viewの設定
///////////////////////////////////////////////////////////////////////////////////////////////

void BaseLight::CalucViewProjection(const Vector3& pos) {
	Matrix4x4 translateMat = pos.MakeTranslateMat();
	Quaternion rot = Quaternion::LookRotation(direction_);
	Matrix4x4 rotateMat = rot.MakeMatrix();
	Matrix4x4 lightMat = Multiply(Multiply(Matrix4x4::MakeUnit(), rotateMat), translateMat);

	Matrix4x4 viewMatrix_ = Inverse(lightMat);
	Matrix4x4 projectionMatrix_ = Matrix4x4::MakePerspectiveFov(fovY_, float(1000.0f) / float(1000.0f), near_, far_);
	viewProjectionMatrix_ = viewMatrix_ * projectionMatrix_;

	data_->view = viewMatrix_;
	data_->projection = projectionMatrix_;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BaseLight::EditParameter(const std::string& name) {
	if (ImGui::CollapsingHeader(name.c_str())) {
		baseParameter_.Debug_Gui();
	}
}

void BaseLight::BaseParameter::Debug_Gui() {
	ImGui::DragFloat3("pos", &lightPos.x, 0.1f);
	ImGui::DragFloat3("direction", &direction.x, 0.1f);
	ImGui::DragFloat("fovY", &fovY, 0.1f);
	ImGui::DragFloat("nearClip", &nearClip, 0.1f);
	ImGui::DragFloat("farClip", &farClip, 0.1f);

	SaveAndLoad();
}
