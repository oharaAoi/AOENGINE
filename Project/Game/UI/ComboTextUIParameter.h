#pragma once
#include <string>

#include "Engine/Lib/Json/IJsonConverter.h"
#include "Engine/System/Editor/Parameter/CustomParameter.h"

#include "Game/UI/PopupTextUI.h"

/// <summary>
/// コンボ表示(ComboTextUI)の調整値。
/// 表示は次々に作られては消えるため、この調整値は ComboTextUIManager が1つだけ持ち、
/// 表示側はそれを参照する(実行中に編集した値がそのまま反映される)
/// </summary>
struct ComboTextUIParameter :
	public AOENGINE::CustomParameterSet,
	public AOENGINE::IJsonConverter {

	// 出る・跳ねる・消えるの調整値。ComboTextUI へはこれを参照させる
	PopupTextParams popup{};

	// コンボ数の後ろに付ける文字。"2" + この文字 が表示される
	std::string textSuffix = " Combo!";

	ComboTextUIParameter() : CustomParameterSet("ComboText") {
		SetGroupName("ComboText");
		SetName("comboTextUI");

		AddSeparatorText("Text");
		AddParameter("Text Suffix", textSuffix);
		AddParameter("Visible Size", popup.visibleSize, 0.01f, 0.0f, 100.0f);

		AddSeparatorText("Fade");
		AddParameter("Fade In Time", popup.fadeInTime, 0.01f, 0.0f, 10.0f);
		AddParameter("Fade Out Time", popup.fadeOutTime, 0.01f, 0.0f, 10.0f);

		AddSeparatorText("Bounce");
		AddParameter("First Velocity", popup.firstVelocity, 0.1f, 0.0f, 100.0f);
		AddParameter("Gravity Rate", popup.gravityRate, 0.01f, 0.0f, 100.0f);
		AddParameter("Damping", popup.damping, 0.01f, 0.0f, 1.0f);
		AddParameter("Target Bounce Count", popup.targetBounceCount, 1.0f, 0.0f, 20.0f);
	}

	json ToJson(const std::string& id) const override {
		return AOENGINE::JsonBuilder(id)
			.Add("textSuffix", textSuffix)
			.Add("visibleSize", popup.visibleSize)
			.Add("fadeInTime", popup.fadeInTime)
			.Add("fadeOutTime", popup.fadeOutTime)
			.Add("firstVelocity", popup.firstVelocity)
			.Add("gravityRate", popup.gravityRate)
			.Add("damping", popup.damping)
			.Add("targetBounceCount", popup.targetBounceCount)
			.Build();
	}

	void FromJson(const json& jsonData) override {
		Convert::fromJson(jsonData, "textSuffix", textSuffix);
		Convert::fromJson(jsonData, "visibleSize", popup.visibleSize);
		Convert::fromJson(jsonData, "fadeInTime", popup.fadeInTime);
		Convert::fromJson(jsonData, "fadeOutTime", popup.fadeOutTime);
		Convert::fromJson(jsonData, "firstVelocity", popup.firstVelocity);
		Convert::fromJson(jsonData, "gravityRate", popup.gravityRate);
		Convert::fromJson(jsonData, "damping", popup.damping);
		Convert::fromJson(jsonData, "targetBounceCount", popup.targetBounceCount);
	}
};
