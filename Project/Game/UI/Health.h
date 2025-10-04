#pragma once
#include "Game/UI/BaseGaugeUI.h"

/// <summary>
/// HpGauge
/// </summary>
class Health :
	public BaseGaugeUI {
public:

	Health() = default;
	~Health() override = default;

	void Init(const std::string& _groupName, const std::string& _itemName);

	void Update(float _fillAmount);

	void Debug_Gui() override;
};

