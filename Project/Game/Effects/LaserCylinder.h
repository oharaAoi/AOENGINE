#pragma once
#include <memory>
// Engine
#include "Engine/Module/Components/GameObject/BaseEntity.h"
#include "Engine/System/ShaderGraph/ShaderGraph.h"

/// <summary>
/// Laserを覆う円柱エフェト
/// </summary>
class LaserCylinder :
	public AOENGINE::BaseEntity {
public: // コンストラクタ

	LaserCylinder() = default;
	~LaserCylinder();

public:

	// 終了処理
	void Finalize() {};

	// 初期化
	void Init(const AOENGINE::Color& _color);

	// 更新
	void Update();

	// 編集
	virtual void Debug_Gui() override;

	void SetUvScale(float _scaleZ);
	void SetScaleZ(float _scaleZ);

public:

	AOENGINE::ShaderGraph* GetShaderGraph() { return shaderGraph_.get(); }

	const std::string& GetShaderGraphPath() const { return shaderGraph_->GetPath(); }

private:

	std::unique_ptr<AOENGINE::ShaderGraph> shaderGraph_;

};

