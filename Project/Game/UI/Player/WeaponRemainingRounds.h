#pragma once
#include <memory>
#include <string>
#include "Engine/Module/Components/2d/Sprite.h"
#include "Engine/Module/Components/Attribute/AttributeGui.h"
#include "Engine/Module/Components/Animation/VectorTween.h"

/// <summary>
/// 武器の残弾数を可視化したクラス
/// </summary>
class WeaponRemainingRounds :
	public AttributeGui {
public:

	WeaponRemainingRounds() = default;
	~WeaponRemainingRounds() override = default;

public:

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="_name"></param>
	void Init(const std::string& _name);

	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="pos"></param>
	/// <param name="_fillAmount"></param>
	void Update(const Vector2& pos, float _fillAmount);

	// 編集
	void Debug_Gui() override;

	// 点滅させる
	void Blinking();

public:

	Sprite* GetSprite() { return gauge_; }

private:

	Sprite* gauge_;
	float timer_;

	VectorTween<Color> colorAnimation_;
};

