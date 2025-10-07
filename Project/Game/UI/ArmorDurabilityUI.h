#pragma once
#include "Game/UI/BaseGaugeUI.h"

/// <summary>
/// アーマーの耐久度
/// </summary>
class ArmorDurabilityUI :
	public BaseGaugeUI {
public:

	ArmorDurabilityUI() = default;
	~ArmorDurabilityUI() override = default;

	void Init(const std::string& _groupName, const std::string& _itemName);

	void Update(float _fillAmount);

	void Debug_Gui() override;

private:

	Sprite* fence_;
};

