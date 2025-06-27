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

	void Init();

	void Update(float _fillAmount);

	void Draw() const override;

	void Debug_Gui() override;

private:

	std::unique_ptr<Sprite> fence_;
};

