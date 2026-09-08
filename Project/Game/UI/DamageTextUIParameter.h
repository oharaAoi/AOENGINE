#pragma once
#include <cstdint>
#include <string>

#include "Engine/Lib/Json/IJsonConverter.h"
#include "Engine/System/Editor/Parameter/CustomParameter.h"

#include "Game/UI/PopupTextUI.h"

/// <summary>
/// 集合中の数と総ダメージの表示(DamageTextUI)の調整値。
/// 表示は集合のたびに作られては消えるため、この調整値は DamageTextUIManager が1つだけ持ち、
/// 表示側はそれを参照する(実行中に編集した値がそのまま反映される)
/// </summary>
struct DamageTextUIParameter :
	public AOENGINE::CustomParameterSet,
	public AOENGINE::IJsonConverter {

	// 出る・跳ねる・消えるの調整値。DamageTextUI へはこれを参照させる
	PopupTextParams popup{};

	// ダメージ量の前後に付ける文字。textPrefix + 数値 + textSuffix が表示される
	std::string textPrefix = "";
	std::string textSuffix = " DAMAGE!";
	// 数値の小数点以下の桁数
	int32_t decimals = 0;

	// 集合中に出す「集めた数」の文字。
	// ブロック数 + blockLabel + countSeparator + コンボ数 + comboLabel が表示される
	std::string blockLabel = " Block";
	std::string countSeparator = "  ";
	std::string comboLabel = " Combo";

	DamageTextUIParameter() : CustomParameterSet("DamageText") {
		SetGroupName("DamageText");
		SetName("damageTextUI");

		AddSeparatorText("Damage Text");
		AddParameter("Text Prefix", textPrefix);
		AddParameter("Text Suffix", textSuffix);
		AddParameter("Decimals", decimals, 1.0f, 0.0f, 3.0f);

		AddSeparatorText("Count Text");
		AddParameter("Block Label", blockLabel);
		AddParameter("Count Separator", countSeparator);
		AddParameter("Combo Label", comboLabel);

		AddSeparatorText("Look");
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
			.Add("textPrefix", textPrefix)
			.Add("textSuffix", textSuffix)
			.Add("decimals", decimals)
			.Add("blockLabel", blockLabel)
			.Add("countSeparator", countSeparator)
			.Add("comboLabel", comboLabel)
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
		Convert::fromJson(jsonData, "textPrefix", textPrefix);
		Convert::fromJson(jsonData, "textSuffix", textSuffix);
		Convert::fromJson(jsonData, "decimals", decimals);
		Convert::fromJson(jsonData, "blockLabel", blockLabel);
		Convert::fromJson(jsonData, "countSeparator", countSeparator);
		Convert::fromJson(jsonData, "comboLabel", comboLabel);
		Convert::fromJson(jsonData, "visibleSize", popup.visibleSize);
		Convert::fromJson(jsonData, "fadeInTime", popup.fadeInTime);
		Convert::fromJson(jsonData, "fadeOutTime", popup.fadeOutTime);
		Convert::fromJson(jsonData, "firstVelocity", popup.firstVelocity);
		Convert::fromJson(jsonData, "gravityRate", popup.gravityRate);
		Convert::fromJson(jsonData, "damping", popup.damping);
		Convert::fromJson(jsonData, "targetBounceCount", popup.targetBounceCount);
	}
};
