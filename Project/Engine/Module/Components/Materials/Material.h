#pragma once
#include "Engine/DirectX/Utilities/DirectXUtils.h"
#include "Engine/Lib/Math/MyMath.h"
#include "Engine/Module/Components/Materials/BaseMaterial.h"
#include "Engine/Module/Components/Materials/MaterialStructures.h"

namespace AOENGINE {

/// <summary>
/// material
/// </summary>
class Material :
	public AOENGINE::BaseMaterial {
public:

	/// <summary>
	/// Gpuに送る構造体
	/// </summary>
	struct MaterialData {
		Math::Vector4 color;
		int32_t enableLighting;
		float pad[3];
		Math::Matrix4x4 uvTransform;
		float shininess;
		float discardValue;
		float iblScale;
	};

public:

	Material() = default;
	~Material() override;

public:

	// 初期化
	void Init() override;
	// 更新
	void Update() override;
	// 編集処理
	void Debug_Gui() override;
	// materialを設定
	void SetMaterialData(ModelMaterialData materialData) override;
	// コマンドを積む
	void BindCommand(ID3D12GraphicsCommandList* _cmdList, const AOENGINE::Pipeline* _pso) override;

public:

	void SetDiscardValue(float _value) { discardValue_ = _value; }

private:

	MaterialData* material_;

};

}