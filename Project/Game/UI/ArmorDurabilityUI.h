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

public:

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="_groupName"></param>
	/// <param name="_itemName"></param>
	void Init(const std::string& _groupName, const std::string& _itemName);

	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="_fillAmount"></param>
	void Update(float _fillAmount);

	/// <summary>
	/// 編集
	/// </summary>
	void Debug_Gui() override;

private:

	Sprite* fence_;
};

