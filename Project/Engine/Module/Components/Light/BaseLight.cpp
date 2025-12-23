#include "BaseLight.h"

using namespace AOENGINE;

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
	lightPos_ = Math::Vector3(0, 200, 0);
	direction_ = Math::Vector3(0, -1, 0);
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

void BaseLight::CalucViewProjection(const Math::Vector3& pos) {
	Math::Vector3 lightDir = Normalize(direction_);

	float shadowWidth = baseParameter_.shadowWidth;
	float shadowHeight = baseParameter_.shadowHeight;

	Math::Vector3 lightPos =
		pos - lightDir * (baseParameter_.shadowDepth * 0.5f);

	Math::Matrix4x4 view =
		Math::Matrix4x4::LookAtLH(lightPos, pos, CVector3::UP);

	Math::Matrix4x4 proj =
		Math::Matrix4x4::MakeOrthograhic(
			-shadowWidth * 0.5f,   // left
			shadowHeight * 0.5f,  // top
			shadowWidth * 0.5f,   // right
			-shadowHeight * 0.5f,  // bottom
			baseParameter_.nearClip,                 // near (必ず 0 以上)
			baseParameter_.farClip           // far
		);

	viewProjectionMatrix_ = view * proj;
	data_->view = view;
	data_->projection = proj;
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
	ImGui::DragFloat("shadowDepth", &shadowDepth, 0.1f);
	ImGui::DragFloat("shadowWidth", &shadowWidth, 0.1f);
	ImGui::DragFloat("shadowHeight", &shadowHeight, 0.1f);

	SaveAndLoad();
}
