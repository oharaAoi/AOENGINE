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

	struct SelectParam : public IJsonConverter {
		
	};

public:

	GameModeGuide();
	~GameModeGuide();

	void Init();
	void Update();

	void Debug_Gui() override;

public:

	void Open();

	bool Decide();

	SelectModeType GetSelectModeType() const { return selectModeType_; }

private:

	std::unique_ptr<Sprite> goGame_;
	std::unique_ptr<Sprite> goTutorial_;
	std::unique_ptr<Sprite> select_;

	SelectModeType selectModeType_;
	int selectModeIndex_;

	bool isSelectCoolTime_;
	float timer_;

	bool isOpen_;

private:

	bool Select();

};

