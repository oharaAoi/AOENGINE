#pragma once
#include <array>
#include <cstddef>
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Engine/Lib/Math/Vector2.h"
#include "Engine/Lib/Math/Vector3.h"

/// <summary>
/// ボスの調整パラメータ
/// </summary>
struct BossParameter : public AOENGINE::IJsonConverter {

	// フェーズ切り替え回数 
	static constexpr std::size_t kPhaseSwitchCount = 2;

	Math::Vector2 screenPos;	// 画面上での固定位置
	float worldZ;
	float hp;	             // 最大HP
	Math::Vector3 hitSize;	// 当たり判定サイズ

	// フェーズが切り替わる残HPの割合
	std::array<float, kPhaseSwitchCount> phaseSwitchRatio;

	BossParameter() {
		SetGroupName("Boss");
		SetName("bossParameter");
	}

	json ToJson(const std::string& id) const override {
		return AOENGINE::JsonBuilder(id)
			.Add("screenPos", screenPos)
			.Add("worldZ", worldZ)
			.Add("hp", hp)
			.Add("hitSize", hitSize)
			.Add("phaseSwitchRatio", json(phaseSwitchRatio))
			.Build();
	}

	void FromJson(const json& jsonData) override {
		Convert::fromJson(jsonData, "screenPos", screenPos);
		Convert::fromJson(jsonData, "worldZ", worldZ);
		Convert::fromJson(jsonData, "hp", hp);
		Convert::fromJson(jsonData, "hitSize", hitSize);

		// 配列を保存
		if (jsonData.is_object() && !jsonData.empty()) {
			const json& body = jsonData.at(jsonData.begin().key());
			if (body.contains("phaseSwitchRatio") && body.at("phaseSwitchRatio").is_array()) {
				const json& array = body.at("phaseSwitchRatio");
				for (std::size_t i = 0; i < phaseSwitchRatio.size() && i < array.size(); ++i) {
					phaseSwitchRatio[i] = array.at(i).get<float>();
				}
			}
		}
	}

	void Debug_Gui() override;
};
