#pragma once
#include "Engine/Module/Components/GameObject/BaseGameObject.h"
#include "Engine/Module/Components//WorldTransform.h"
#include "Engine/Module/Components/Attribute/AttributeGui.h"

/// <summary>
/// 床
/// </summary>
class Floor :
	public AttributeGui {
public:

	Floor();
	~Floor();

public:

	// 初期化
	void Init();
	// 編集
	void Debug_Gui() override;

private:

	BaseGameObject* floor_;
	WorldTransform* transform_;

};

