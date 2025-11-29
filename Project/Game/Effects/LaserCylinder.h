#pragma once
#include <memory>
// Engine
#include "Engine/Module/Components/GameObject/BaseEntity.h"
#include "Engine/System/ShaderGraph/ShaderGraph.h"

/// <summary>
/// Laserを覆う円柱エフェト
/// </summary>
class LaserCylinder :
	public BaseEntity {
public: // コンストラクタ

	LaserCylinder() = default;
	~LaserCylinder();

public:

	// 終了処理
	void Finalize() {};

	// 初期化
	void Init();

	// 更新
	void Update();

	// 編集
	virtual void Debug_Gui() override;

	void SetUvScale(float _scaleZ);
	void SetScaleZ(float _scaleZ);

private:

	std::unique_ptr<ShaderGraph> shaderGraph_;

};

