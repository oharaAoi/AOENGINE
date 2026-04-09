#pragma once
// c++

// engine
#include "Engine/Module/Components/Materials/BaseMaterial.h"
#include "Engine/DirectX/Pipeline/Parts/Blend.h"

enum class MaterialType {
	Normal,
	PBR
};

namespace AOENGINE {

/// <summary>
/// Materialを管理するクラス
/// </summary>
class MaterialManager {
public: // コンストラクタ

	MaterialManager();
	~MaterialManager();

public: // public method

	/// <summary>
	/// 初期化処理
	/// </summary>
	void Init();

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();



};

}