#pragma once
#include "Game/UI/BaseGaugeUI.h"

/// <summary>
/// 姿勢安定のゲージ
/// </summary>
class PostureStability :
	public BaseGaugeUI {
public:

	PostureStability() = default;
	~PostureStability() override = default;

	void Init();
	
	void Update(float _fillAmount);

	void Draw() const override;

	void Debug_Gui() override;

private:

	std::unique_ptr<Sprite> fence_;

};

