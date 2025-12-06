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

	// 初期化
	void Init();
	// 更新
	void Update();
	// 編集
	void Debug_Gui() override;
	// 塗りつぶし量
	void FillAmountGauge(float _fillAmount);
	// 成功
	void Success(bool _isSuccess);
	// チャージゲージ
	void ChangeControlUI(const std::string& _fileName);

public:

	bool GetIsSuccessFinish() const { return isSuccessFinish_; }

private:

	AOENGINE::Sprite* gauge_;
	AOENGINE::Sprite* gaugeBg_;
	AOENGINE::Sprite* success_;

	AOENGINE::Sprite* control_;
	AOENGINE::Sprite* controlBg_;

	VectorTween<float> alphaAnimation_;
	bool isSuccessFinish_ = false;
};

