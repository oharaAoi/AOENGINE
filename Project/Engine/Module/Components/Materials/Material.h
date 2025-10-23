#pragma once
#include "Engine/DirectX/Utilities/DirectXUtils.h"
#include "Engine/Lib/Math/MyMath.h"
#include "Engine/Module/Components/Materials/BaseMaterial.h"
#include "Engine/Module/Components/Materials/MaterialStructures.h"

/// <summary>
/// material
/// </summary>
class Material :
	public BaseMaterial {
public:

	struct MaterialData {
		Vector4 color;
		int32_t enableLighting;
		float pad[3];
		Matrix4x4 uvTransform;
		float shininess;
		float discardValue;
	};

public:

	Material() = default;
	~Material() override;

public:

	// 初期化
	void Init() override;
	// 更新
	void Update() override;
	// コマンドを積む
	void SetCommand(ID3D12GraphicsCommandList* commandList) override;
	// 編集処理
	void Debug_Gui() override;
	// materialを設定
	void SetMaterialData(ModelMaterialData materialData) override;

public:

	void SetDiscardValue(float _value) { material_->discardValue = _value; }

private:

	MaterialData* material_;

};