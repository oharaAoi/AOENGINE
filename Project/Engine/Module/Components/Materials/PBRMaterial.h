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

namespace AOENGINE {

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
		AOENGINE::Color color;				// albedo
		int32_t enableLighting;
		float pad[3];
		Math::Matrix4x4 uvTransform;
		AOENGINE::Color diffuseColor;		// 拡散反射率
		AOENGINE::Color specularColor;		// 鏡面反射の色
		float roughness;			// 粗さ
		float metallic;				// 金属度
		float shininess;			// 鋭さ
		float ambientIntensity;		// 環境光の強さ
	};

public:

	PBRMaterial();
	~PBRMaterial() override;

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

	void SetColor(const AOENGINE::Color& color) { pbrMaterial_->color = color; }

	void SetParameter(float _roughness, float _metallic, float _ibl);


private:

	PBRMaterialData* pbrMaterial_;
};

}