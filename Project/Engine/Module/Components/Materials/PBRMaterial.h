#pragma once
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <map>
#include "Engine/DirectX/Utilities/DirectXUtils.h"
#include "Engine/Lib/Color.h"
#include "Engine/Lib/Math/MyMath.h"
#include "Engine/Module/Components/GameObject/Model.h"
#include "Engine/Module/Components/Materials/BaseMaterial.h"
#include "Engine/Module/Components/Materials/MaterialStructures.h"

/// <summary>
/// 物理ベースmaterial
/// </summary>
class PBRMaterial :
	public BaseMaterial {
public:

	/// <summary>
	/// Gpuに送る構造体
	/// </summary>
	struct PBRMaterialData {
		Color color;				// albedo
		int32_t enableLighting;
		float pad[3];
		Matrix4x4 uvTransform;
		Color diffuseColor;		// 拡散反射率
		Color specularColor;		// 鏡面反射の色
		float roughness;			// 粗さ
		float metallic;				// 金属度
		float shininess;			// 鋭さ
	};

public:

	PBRMaterial();
	~PBRMaterial();

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

	void SetColor(const Color& color) { pbrMaterial_->color = color; }


private:

	PBRMaterialData* pbrMaterial_;
};

