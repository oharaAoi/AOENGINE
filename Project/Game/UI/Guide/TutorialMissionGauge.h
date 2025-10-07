#pragma once
#include <memory>
#include <string>
#include "Engine/Module/Components/2d/Sprite.h"
#include "Engine/Module/Components/Attribute/AttributeGui.h"
#include "Engine/Module/Components/Animation/VectorTween.h"

/// <summary>
/// TutorialのMissionの進行度を表すゲージ
/// </summary>
class TutorialMissionGauge :
	public AttributeGui {
public:

	TutorialMissionGauge() = default;
	~TutorialMissionGauge() = default;

public:

	void Init();
	void Update();

	void Debug_Gui() override;

	void FillAmountGauge(float _fillAmount);

	void Success(bool _isSuccess);

	void ChangeControlUI(const std::string& _fileName);

public:

	bool GetIsSuccessFinish() const { return isSuccessFinish_; }

private:

	std::unique_ptr<Sprite> gauge_;
	std::unique_ptr<Sprite> gaugeBg_;
	std::unique_ptr<Sprite> success_;

	std::unique_ptr<Sprite> control_;
	std::unique_ptr<Sprite> controlBg_;

	VectorTween<float> alphaAnimation_;
	bool isSuccessFinish_ = false;
};

