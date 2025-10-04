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

	virtual void Debug_Gui() override;

public:	// accessor method

	void SetFillAmount(float _amount) { fillAmount_ = _amount; }

	void SetIsEnable(bool _isActive);

	void SetGroupName(const std::string& _groupName) { groupName_ = _groupName; }
	const std::string& GetGroupName() const { return groupName_; }

protected:

	std::unique_ptr<Sprite> bg_;
	std::unique_ptr<Sprite> front_;

	float fillAmount_;

	std::string groupName_;

};

