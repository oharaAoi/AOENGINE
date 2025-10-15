#pragma once
#include <string>
#include "Engine/Lib/Math/Curve.h"
#include "Engine/Lib/Json/IJsonConverter.h"

/// <summary>
/// 評価関数となるクラス
/// </summary>
struct UtilityEvaluator : IJsonConverter {

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

	json ToJson(const std::string& id) const override {
		json curveJson = curve.ToJson();
		return JsonBuilder(id)
			.Add("name", name_)
			.Add("weight", weight)
			.Add("maxValue", maxValue)
			.Add("decelerationCurve", curveJson)
			.Build();
	}

	void FromJson(const json& jsonData) override {
		fromJson(jsonData, "name", name_);
		fromJson(jsonData, "weight", weight);
		fromJson(jsonData, "maxValue", maxValue);
		curve.FromJson(jsonData, "decelerationCurve");
	}

	void Debug_Gui() override;

	void LoadJson(const std::string& _group, const std::string& _key);

};

