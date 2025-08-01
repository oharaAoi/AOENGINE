#pragma once
#include <Windows.h>
#include <cstdint>
#include <d3d12.h>
#include <wrl.h>
#include <filesystem>
#include <string>
#include <vector>
#include "Engine/DirectX/Utilities/DirectXUtils.h"
#include "Engine/Lib/Math/MyMath.h"
#include "Engine/Module/Components/GameObject/Model.h"

template<typename T>
using ComPtr = Microsoft::WRL::ComPtr <T>;

class Material {
public:

	struct MaterialData {
		Vector4 color;
		int32_t enableLighting;
		float pad[3];
		Matrix4x4 uvTransform;
		float shininess;
	};

	struct SpriteData {
		Matrix4x4 uvTransform;
		Vector4 color;
	};

public:

	Material();
	~Material();

	void Finalize();
	void Init(ID3D12Device* device, const Model::ModelMaterialData& materialData);
	void Update();

	void Draw(ID3D12GraphicsCommandList* commandList);

	void Debug_Gui();

private:

	void SelectTexture();

public:

	D3D12_GPU_VIRTUAL_ADDRESS GetBufferAdress() const { return materialBuffer_->GetGPUVirtualAddress(); }

	Model::ModelMaterialData GetBaseMaterial() { return materialsData_; }
	const std::string GetUseTexture() const { return materialsData_.textureFilePath; }
	void SetUseTexture(const std::string& name);

	void SetColor(const Vector4& color) { material_->color = color; }

	void SetAlpha(float alpha) { material_->color.w = alpha; }

	void SetIsLighting(bool isLighting){ material_->enableLighting = isLighting; };

	void SetMaterialData(Model::ModelMaterialData materialData);

	void SetUvScale(const Vector3& scale) { uvScale_ = scale; }
	void SetUvRotate(const Vector3& rotate) { uvRotation_ = rotate; }
	void SetUvTranslation(const Vector3& translation) { uvTranslation_ = translation; }

private:

	ComPtr<ID3D12Resource> materialBuffer_;
	MaterialData* material_;

	Model::ModelMaterialData materialsData_;

	Vector3 uvTranslation_;
	Vector3 uvScale_;
	Vector3 uvRotation_;

};