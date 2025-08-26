#pragma once
#include <string>
#include "Engine/DirectX/Utilities/DirectXUtils.h"
#include "Engine/Module/Components/Materials/MaterialStructures.h"
#include "Engine/Lib/Math/MathStructures.h"

class BaseMaterial {
public:

	BaseMaterial() = default;
	virtual ~BaseMaterial() = default;

	virtual void Init() = 0;

	virtual void Update() = 0;

	virtual void SetCommand(ID3D12GraphicsCommandList* commandList) = 0;

	virtual void Debug_Gui() = 0;

	virtual void SetMaterialData(ModelMaterialData materialData) = 0;

protected:

	void Debug_UV();

public:

	D3D12_GPU_VIRTUAL_ADDRESS GetBufferAddress() const { return cBuffer_->GetGPUVirtualAddress(); }

	void SetUvScale(const Vector3& _scale) { uvTransform_.scale = _scale; }
	void SetUvRotate(const Vector3& _rotate) { uvTransform_.rotate = _rotate; }
	void SetUvTranslate(const Vector3& _translate) { uvTransform_.translate = _translate; }

	void SetAlbedoTexture(const std::string& _name) { textureName_ = _name; }
	const std::string GetAlbedoTexture() const { return textureName_; }

	void SetColor(const Vector4& _color) { color_ = _color; }
	const Vector4 GetColor() const { return color_; }

	void SetAlpha(float _alpha) { color_.w = _alpha; };
	void SetIsLighting(bool _isLighting) { isLighting_ = _isLighting; }

protected:

	ComPtr<ID3D12Resource> cBuffer_;

	SRT uvTransform_ = SRT();
	std::string textureName_ = "white.png";

	Vector4 color_;
	bool isLighting_;
};

