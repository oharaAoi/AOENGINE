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

#ifdef _DEBUG
	void Debug_Gui() override;
#endif

private:



};

