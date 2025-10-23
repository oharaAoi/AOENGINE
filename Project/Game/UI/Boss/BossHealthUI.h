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
public:

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="_groupName">: group名</param>
	/// <param name="_itemName">: item名</param>
	void Init(const std::string& _groupName, const std::string& _itemName);

	/// <summary>
	/// 更新処理
	/// </summary>
	/// <param name="_fillAmount">: 塗りつぶし量</param>
	void Update(float _fillAmount);

	/// <summary>
	/// 編集処理
	/// </summary>
	void Debug_Gui() override;

};

