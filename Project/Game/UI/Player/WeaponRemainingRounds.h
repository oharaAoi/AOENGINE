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

	void Init(const std::string& _name);

	void Update(const Vector2& pos, float _fillAmount);

	void Debug_Gui() override;

	// 点滅させる
	void Blinking();

public:

	Sprite* GetSprite() { return gauge_.get(); }

private:

	std::unique_ptr<Sprite> gauge_;
	float timer_;

	VectorTween<Vector4> colorAnimation_;
};

