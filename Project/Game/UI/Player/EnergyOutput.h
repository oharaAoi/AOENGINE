#pragma once
#include "Game/UI/BaseGaugeUI.h"

/// <summary>
/// EN出力のゲージ
/// </summary>
class EnergyOutput :
	public BaseGaugeUI {
public:

	EnergyOutput() = default;
	~EnergyOutput() override = default;

	void Init();

	void Update(float _fillAmount);

	void Draw() const override;

#ifdef _DEBUG
	void Debug_Gui() override;
#endif
};

