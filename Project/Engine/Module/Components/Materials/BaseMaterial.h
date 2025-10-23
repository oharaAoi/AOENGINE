#pragma once
#include <string>
#include "Engine/DirectX/Utilities/DirectXUtils.h"
#include "Engine/Module/Components/Materials/MaterialStructures.h"
#include "Engine/Lib/Color.h"
#include "Engine/Lib/Math/MathStructures.h"

/// <summary>
/// BaseとなるMaterial
/// </summary>
class BaseMaterial {
public:

	BaseMaterial() = default;
	virtual ~BaseMaterial() = default;

public:

	// 初期化
	virtual void Init() = 0;
	// 更新
	virtual void Update() = 0;
	// コマンドを積む
	virtual void SetCommand(ID3D12GraphicsCommandList* commandList) = 0;
	// 編集処理
	virtual void Debug_Gui() = 0;
	// materialの設定
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

	void SetColor(const Color& _color) { color_ = _color; }
	const Color GetColor() const { return color_; }

	void SetAlpha(float _alpha) { color_.a = _alpha; };
	void SetIsLighting(bool _isLighting) { isLighting_ = _isLighting; }

protected:

	ComPtr<ID3D12Resource> cBuffer_;

	SRT uvTransform_ = SRT();
	std::string textureName_ = "white.png";

	Color color_;
	bool isLighting_;
};

