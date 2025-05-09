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

#ifdef _DEBUG
	virtual void Debug_Gui() override;
#endif

public:	// accessor method

	void SetFillAmount(float _amount) { fillAmount_ = _amount; }

protected:

	std::unique_ptr<Sprite> bg_;
	std::unique_ptr<Sprite> front_;

	Vector2 centerPos_;
	float fillAmount_;

};

