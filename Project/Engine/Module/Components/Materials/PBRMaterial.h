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

class PBRMaterial :
	public BaseMaterial {
public:

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

	void Init() override;

	void Update() override;

	void SetCommand(ID3D12GraphicsCommandList* commandList) override;

	void Debug_Gui() override;

	void SetMaterialData(ModelMaterialData materialData) override;

public:

	void SetColor(const Color& color) { pbrMaterial_->color = color; }


private:

	PBRMaterialData* pbrMaterial_;
};

