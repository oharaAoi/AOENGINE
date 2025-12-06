#pragma once
#include <memory>
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Engine/Module/Components/2d/Sprite.h"
#include "Engine/Module/Components/Attribute/AttributeGui.h"

enum SelectModeType {
	ToGame,
	ToTutorial,
	kMax
};

/// <summary>
/// GameModeを選択するUI
/// </summary>
class GameModeGuide :
	public AttributeGui {
public:

	GameModeGuide();
	~GameModeGuide();

public:

	// 初期化
	void Init();
	// 更新
	void Update();
	// 編集
	void Debug_Gui() override;

public:

	// 開く
	void Open();
	// 決定
	bool Decide();

	SelectModeType GetSelectModeType() const { return selectModeType_; }

private:

	AOENGINE::Sprite* goGame_;
	AOENGINE::Sprite* goTutorial_;
	AOENGINE::Sprite* select_;

	SelectModeType selectModeType_;
	int selectModeIndex_;

	bool isSelectCoolTime_;
	float timer_;

	bool isOpen_;

private:

	bool Select();

};

