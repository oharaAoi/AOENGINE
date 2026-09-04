#pragma once
#include <array>
#include <cstddef>
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Engine/System/Editor/Parameter/CustomParameter.h"
#include "Engine/Lib/Math/Vector2.h"
#include "Engine/Lib/Math/Vector3.h"

/// <summary>
/// ボスの調整パラメータ
/// </summary>
struct BossParameter :
	public AOENGINE::CustomParameterSet,
	public AOENGINE::IJsonConverter {

	// フェーズ切り替え回数
	static constexpr std::size_t kPhaseSwitchCount = 2;

	Math::Vector2 screenPos;	// 画面上での固定位置
	float worldZ;
	float hp;	             // 最大HP
	Math::Vector3 hitSize;	// 当たり判定サイズ

	// 火球落とし中の上下跳ね
	float bounceHeight = 1.0f;	// 跳ねる高さ
	float bounceSpeed = 8.0f;	// 跳ねる速さ

	// 火球の生成位置
	float fireballSpawnOffsetY = 0.0f;	// ボスの高さからさらに上下させるオフセット

	// フェーズが切り替わる残HPの割合
	std::array<float, kPhaseSwitchCount> phaseSwitchRatio;

	BossParameter() : CustomParameterSet("Boss") {
		SetGroupName("Boss");
		SetName("bossParameter");

		AddParameter("Screen Pos(px)", screenPos, 1.0f);
		AddParameter("World Z", worldZ, 0.1f);
		AddParameter("Max HP", hp, 1.0f, 0.0f, 100000.0f);
		AddParameter("Hit Size (half)", hitSize, 0.1f);
		AddParameter("Bounce Height", bounceHeight, 0.1f, 0.0f, 100.0f);
		AddParameter("Bounce Speed", bounceSpeed, 0.1f, 0.0f, 100.0f);
		AddParameter("Fireball Spawn OffsetY", fireballSpawnOffsetY, 0.1f, -100.0f, 100.0f);
		AddParameter("Phase Switch Ratio 0", phaseSwitchRatio[0], 0.01f, 0.0f, 1.0f);
		AddParameter("Phase Switch Ratio 1", phaseSwitchRatio[1], 0.01f, 0.0f, 1.0f);
	}

	json ToJson(const std::string& id) const override {
		return AOENGINE::JsonBuilder(id)
			.Add("screenPos", screenPos)
			.Add("worldZ", worldZ)
			.Add("hp", hp)
			.Add("hitSize", hitSize)
			.Add("bounceHeight", bounceHeight)
			.Add("bounceSpeed", bounceSpeed)
			.Add("fireballSpawnOffsetY", fireballSpawnOffsetY)
			.Add("phaseSwitchRatio", json(phaseSwitchRatio))
			.Build();
	}

	void FromJson(const json& jsonData) override {
		Convert::fromJson(jsonData, "screenPos", screenPos);
		Convert::fromJson(jsonData, "worldZ", worldZ);
		Convert::fromJson(jsonData, "hp", hp);
		Convert::fromJson(jsonData, "hitSize", hitSize);
		Convert::fromJson(jsonData, "bounceHeight", bounceHeight);
		Convert::fromJson(jsonData, "bounceSpeed", bounceSpeed);
		Convert::fromJson(jsonData, "fireballSpawnOffsetY", fireballSpawnOffsetY);

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
};
