#include "PBRMaterial.h"
#include "Engine/System/Manager/ImGuiManager.h"
#include "Engine/System/Manager/TextureManager.h"
#include "Engine/Core/GraphicsContext.h"

using namespace AOENGINE;

PBRMaterial::PBRMaterial() {}
PBRMaterial::~PBRMaterial() { cBuffer_.Reset(); }

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void PBRMaterial::Init() {
	AOENGINE::GraphicsContext* ctx = AOENGINE::GraphicsContext::GetInstance();

	cBuffer_ = CreateBufferResource(ctx->GetDevice(), sizeof(PBRMaterialData));
	cBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&pbrMaterial_));

	pbrMaterial_->color = AOENGINE::Color(1.0f, 1.0f, 1.0f, 1.0f);
	pbrMaterial_->enableLighting = false;
	pbrMaterial_->uvTransform = Math::Matrix4x4::MakeUnit();

	pbrMaterial_->diffuseColor = AOENGINE::Color(1.0f, 1.0f, 1.0f, 1.0f);
	pbrMaterial_->specularColor = AOENGINE::Color(1.0f, 1.0f, 1.0f, 1.0f);

	pbrMaterial_->roughness = 0.5f;
	pbrMaterial_->metallic = 0.5f;
	pbrMaterial_->shininess = 10;
	pbrMaterial_->ambientIntensity = 0.5f;

	uvTransform_.scale = CVector3::UNIT;
	uvTransform_.rotate = CVector3::ZERO;
	uvTransform_.translate = CVector3::ZERO;
	pbrMaterial_->uvTransform= uvTransform_.MakeAffine();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void PBRMaterial::Update() {
	pbrMaterial_->uvTransform = uvTransform_.MakeAffine();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集処理
///////////////////////////////////////////////////////////////////////////////////////////////

void PBRMaterial::Debug_Gui() {
	EditUV();
	ImGui::DragFloat("roughness", &pbrMaterial_->roughness, 0.01f, 0.0f, 1.0f);
	ImGui::DragFloat("metallic", &pbrMaterial_->metallic, 0.01f, 0.0f, 1.0f);
	ImGui::DragFloat("ambientIntensity", &pbrMaterial_->ambientIntensity, 0.01f, 0.0f, 1.0f);
}

void PBRMaterial::SetMaterialData(ModelMaterialData materialData) {
	textureName_ = materialData.textureFilePath;

	pbrMaterial_->color = AOENGINE::Color(1.0f, 1.0f, 1.0f, 1.0f);
	pbrMaterial_->enableLighting = true;
	pbrMaterial_->uvTransform = Math::Matrix4x4::MakeUnit();

	pbrMaterial_->diffuseColor = AOENGINE::Color(1.0f, 1.0f, 1.0f, 1.0f);
	pbrMaterial_->specularColor = AOENGINE::Color(1.0f, 1.0f, 1.0f, 1.0f);

	pbrMaterial_->roughness = 1.0f;
	pbrMaterial_->metallic = 1.0f;
	pbrMaterial_->shininess = 50;
	pbrMaterial_->ambientIntensity = 0.5f;
}

void AOENGINE::PBRMaterial::BindCommand(ID3D12GraphicsCommandList* _cmdList, const AOENGINE::Pipeline* _pso) {
	UINT index = _pso->GetRootSignatureIndex("gMaterial");
	_cmdList->SetGraphicsRootConstantBufferView(index, this->GetBufferAddress());

	index = _pso->GetRootSignatureIndex("gTexture");
	if (shaderType_ == MaterialShaderType::UniversalRender) {
		std::string textureName = this->GetAlbedoTexture();
		AOENGINE::TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(_cmdList, textureName, index);
	} else if (shaderType_ == MaterialShaderType::ShaderGraphRender) {
		if (this->GetShaderGraph()) {
			AOENGINE::DxResource* dxResource = this->GetShaderGraph()->GetResource();
			if (dxResource) {
				ID3D12Resource* resource = dxResource->GetResource();
				if (resource) {
					_cmdList->SetGraphicsRootDescriptorTable(index, dxResource->GetSRV().handleGPU);
				} else {
					AOENGINE::TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(_cmdList, "error.png", index);
				}
			} else {
				AOENGINE::TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(_cmdList, "error.png", index);
			}
		} else {
			AOENGINE::TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(_cmdList, "error.png", index);
		}
	}
}

void AOENGINE::PBRMaterial::SetParameter(float _roughness, float _metallic, float _ibl) {
	pbrMaterial_->roughness = _roughness;
	pbrMaterial_->metallic = _metallic;
	pbrMaterial_->ambientIntensity = _ibl;
}
