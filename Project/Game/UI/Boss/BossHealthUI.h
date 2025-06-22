#pragma once
#include "Game/UI/BaseGaugeUI.h"

/// <summary>
/// BossのHPのUI
/// </summary>
class BossHealthUI :
	public BaseGaugeUI {
public:

	BossHealthUI() = default;
	~BossHealthUI() override = default;

	void Init();

	void Update(float _fillAmount);

	void Draw() const override;

	void Debug_Gui() override;

};

