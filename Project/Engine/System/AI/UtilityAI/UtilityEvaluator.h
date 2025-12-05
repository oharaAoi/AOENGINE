#pragma once
#include <string>
#include "Engine/Lib/Math/Curve.h"
#include "Engine/Lib/Json/IJsonConverter.h"

/// <summary>
/// 評価関数となるクラス
/// </summary>
struct UtilityEvaluator : AOENGINE::IJsonConverter {

	UtilityEvaluator() = default;

	float weight = 1.0f;
	float maxValue = 50.0f;
	Curve curve;

	/// <summary>
	/// 評価式
	/// </summary>
	/// <param name="_inputValue">: 評価対象のデータ</param>
	/// <returns></returns>
	float Evaluate(float _inputValue) const;

	/// <summary>
	/// jsonへ
	/// </summary>
	/// <param name="id"></param>
	/// <returns></returns>
	json ToJson(const std::string& id) const override {
		json curveJson = curve.ToJson();
		return AOENGINE::JsonBuilder(id)
			.Add("name", name_)
			.Add("weight", weight)
			.Add("maxValue", maxValue)
			.Add("decelerationCurve", curveJson)
			.Build();
	}

	/// <summary>
	/// jsonから
	/// </summary>
	/// <param name="jsonData"></param>
	void FromJson(const json& jsonData) override {
		fromJson(jsonData, "name", name_);
		fromJson(jsonData, "weight", weight);
		fromJson(jsonData, "maxValue", maxValue);
		curve.FromJson(jsonData, "decelerationCurve");
	}

	/// <summary>
	/// 編集処理
	/// </summary>
	void Debug_Gui() override;

	/// <summary>
	/// jsondataを読み込む
	/// </summary>
	/// <param name="_group"></param>
	/// <param name="_key"></param>
	void LoadJson(const std::string& _group, const std::string& _key);

};

