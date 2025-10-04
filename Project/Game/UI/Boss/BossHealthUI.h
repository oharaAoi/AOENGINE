#pragma once
#include <string>
#include "Game/UI/BaseGaugeUI.h"

/// <summary>
/// BossのHPのUI
/// </summary>
class BossHealthUI :
	public BaseGaugeUI {
public:

	BossHealthUI() = default;
	~BossHealthUI() override = default;

	void Init(const std::string& _groupName, const std::string& _itemName);

	void Update(float _fillAmount);

	void Debug_Gui() override;

};

