#pragma once
#include <memory>
#include "Engine/Module/Components/2d/Sprite.h"
#include "Engine/Module/Components/Attribute/AttributeGui.h"

/// <summary>
/// タイトルのUIをまとめたクラス
/// </summary>
class TitleUIs :
	public AOENGINE::AttributeGui {
public:

	TitleUIs() = default;
	~TitleUIs() = default;

public:

	// 初期化
	void Init();
	// 更新
	void Update();
	// 描画
	void Draw() const;
	// 編集
	void Debug_Gui() override;

private:

	AOENGINE::Sprite* gameTitle_;

};

