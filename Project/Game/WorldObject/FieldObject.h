#pragma once
// Engine
#include "Engine/Module/Components/GameObject/BaseEntity.h"

/// <summary>
/// フィールドに配置されているobject
/// </summary>
class FieldObject :
	public BaseEntity {
public:

	FieldObject() = default;
	~FieldObject() override = default;

public:

	// 初期化
	void Init();
	// 更新
	void Update();
	// 描画
	void Draw() const;
	// 編集
	void Debug_Gui() override;

};

