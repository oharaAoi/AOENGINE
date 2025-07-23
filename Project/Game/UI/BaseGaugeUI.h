#pragma once
#include <memory>
#include <string>
#include "Engine/Module/Components/2d/Sprite.h"
#include "Engine/Module/Components/Attribute/AttributeGui.h"

/// <summary>
/// ゲージを管理する描画する際の元となるクラス
/// </summary>
class BaseGaugeUI :
	public AttributeGui {
public: // base

	BaseGaugeUI() = default;
	virtual ~BaseGaugeUI() = default;

	void Init(const std::string& bgTexture, const std::string& frontTexture);

	void Update();

	virtual void Draw() const;

	virtual void Debug_Gui() override;

public:	// accessor method

	void SetFillAmount(float _amount) { fillAmount_ = _amount; }

	void SetCenterPos(const Vector2& pos) { centerPos_ = pos; }
	const Vector2& GetCenterPos() const { return centerPos_; }

	void SetScale(const Vector2& scale) { scale_ = scale; }
	const Vector2& GetScale() const { return scale_; }

	void SetIsEnable(bool _isActive);

protected:

	std::unique_ptr<Sprite> bg_;
	std::unique_ptr<Sprite> front_;

	Vector2 scale_ = CVector2::UNIT;
	Vector2 centerPos_ = CVector2::ZERO;
	float fillAmount_;

	int fillMoveType_ = 0;

};

