#pragma once
#include <memory>
// light
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Engine/DirectX/Pipeline/Pipeline.h"
#include "Engine/Module/Components/Attribute/AttributeGui.h"

/// <summary>
/// lightをまとめたクラス
/// </summary>
class LightGroup : 
	public AttributeGui {
public:

	enum LightKind {
		Directional,
		PBR,
	};

public:

	LightGroup();
	~LightGroup();

public:

	// 初期化
	void Init(ID3D12Device* device);
	// 終了
	void Finalize();
	// 更新
	void Update();
	// コマンドを積む
	void BindCommand(const Pipeline* pso, ID3D12GraphicsCommandList* commandList);
	// 編集処理
	void Debug_Gui() override;
	// 保存項目を読み込む
	void Load();

public:

	DirectionalLight* GetDirectionalLight() { return directionalLight_.get(); }
	PointLight* GetPointLight() { return pointLight_.get(); }
	SpotLight* GetSpotLight() { return spotLight_.get(); }

public:

	void SetEyePos(const Vector3& pos) { eyePos_ = pos; }

private:

	std::unique_ptr<DirectionalLight> directionalLight_ = nullptr;
	std::unique_ptr<PointLight> pointLight_ = nullptr;
	std::unique_ptr<SpotLight> spotLight_ = nullptr;
	
	Vector3 eyePos_;
};