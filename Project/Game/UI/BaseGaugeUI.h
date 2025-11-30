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

public:

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="bgTexture"></param>
	/// <param name="frontTexture"></param>
	void Init(const std::string& bgTexture, const std::string& frontTexture);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 編集
	/// </summary>
	virtual void Debug_Gui() override;

public:	// accessor method

	Sprite* GetFront() const { return front_; }
	
	Sprite* GetBg() const { return bg_; }

	void SetPos(const Vector2& _pos);

	void SetFillAmount(float _amount) { fillAmount_ = _amount; }

	void SetIsEnable(bool _isActive);
	bool GetIsEnable() const;

	void SetGroupName(const std::string& _groupName) { groupName_ = _groupName; }
	const std::string& GetGroupName() const { return groupName_; }

protected:

	Sprite* bg_;
	Sprite* front_;

	float fillAmount_;

	std::string groupName_;

};

