#pragma once
#include <array>
#include <cstddef>
#include <cstdint>
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

	// --- ボス本体 ---
	Math::Vector2 screenPos;	// 画面上での固定位置
	float worldZ;
	float hp;	             // 最大HP
	Math::Vector3 hitSize;	// 当たり判定サイズ

	// フェーズが切り替わる残HPの割合
	std::array<float, kPhaseSwitchCount> phaseSwitchRatio;

	// --- 攻撃1: 火球落とし ---
	float fireballSpawnOffsetY = 0.0f;	// ボスの高さからさらに上下させるオフセット
	float bounceHeight = 1.0f;			// 跳ねる高さ
	float bounceSpeed = 8.0f;			// 跳ねる速さ

	// --- 攻撃2: ビーム ---
	int32_t beamCount = 3;					// ビーム回数
	float beamWarningTime = 1.5f;			// 予測線秒数
	float beamTime = 1.0f;					// ビーム秒数
	float beamSpeed = 40.0f;				// ビームが横切る速さ
	float beamStartX = -30.0f;				// ビームを出す画面外のX
	float beamBlinkInterval = 0.1f;			// 予測線の点滅間隔
	Math::Vector3 beamSize{ 40.0f, 1.2f, 1.0f };		// ビームの大きさ
	Math::Vector3 beamWarningLineSize{ 40.0f, 1.2f, 1.0f };	// 予測線の大きさ
	Math::Vector3 beamWarningMarkSize{ 1.5f, 1.5f, 1.0f };	// 危険マークの大きさ

	BossParameter() : CustomParameterSet("Boss") {
		SetGroupName("Boss");
		SetName("bossParameter");

		AddSeparatorText("Boss");
		AddParameter("Screen Pos(px)", screenPos, 1.0f);
		AddParameter("World Z", worldZ, 0.1f);
		AddParameter("Max HP", hp, 1.0f, 0.0f, 100000.0f);
		AddParameter("Hit Size (half)", hitSize, 0.1f);
		AddParameter("Phase Switch Ratio 0", phaseSwitchRatio[0], 0.01f, 0.0f, 1.0f);
		AddParameter("Phase Switch Ratio 1", phaseSwitchRatio[1], 0.01f, 0.0f, 1.0f);

		AddSeparatorText("Attack1: FallFire");
		AddParameter("Fireball Spawn OffsetY", fireballSpawnOffsetY, 0.1f, -100.0f, 100.0f);
		AddParameter("Bounce Height", bounceHeight, 0.1f, 0.0f, 100.0f);
		AddParameter("Bounce Speed", bounceSpeed, 0.1f, 0.0f, 100.0f);

		AddSeparatorText("Attack2: Beam");
		AddParameter("Beam Count", beamCount, 1.0f, 0.0f, 100.0f);
		AddParameter("Beam Warning Time", beamWarningTime, 0.01f, 0.0f, 10.0f);
		AddParameter("Beam Time", beamTime, 0.01f, 0.0f, 10.0f);
		AddParameter("Beam Speed", beamSpeed, 0.1f, 0.0f, 1000.0f);
		AddParameter("Beam Start X", beamStartX, 0.1f, -1000.0f, 1000.0f);
		AddParameter("Beam Blink Interval", beamBlinkInterval, 0.01f, 0.0f, 5.0f);
		AddParameter("Beam Size", beamSize, 0.1f);
		AddParameter("Warning Line Size", beamWarningLineSize, 0.1f);
		AddParameter("Warning Mark Size", beamWarningMarkSize, 0.1f);
	}

	json ToJson(const std::string& id) const override {
		return AOENGINE::JsonBuilder(id)
			.Add("screenPos", screenPos)
			.Add("worldZ", worldZ)
			.Add("hp", hp)
			.Add("hitSize", hitSize)
			.Add("phaseSwitchRatio", json(phaseSwitchRatio))
			.Add("fireballSpawnOffsetY", fireballSpawnOffsetY)
			.Add("bounceHeight", bounceHeight)
			.Add("bounceSpeed", bounceSpeed)
			.Add("beamCount", beamCount)
			.Add("beamWarningTime", beamWarningTime)
			.Add("beamTime", beamTime)
			.Add("beamSpeed", beamSpeed)
			.Add("beamStartX", beamStartX)
			.Add("beamBlinkInterval", beamBlinkInterval)
			.Add("beamSize", beamSize)
			.Add("beamWarningLineSize", beamWarningLineSize)
			.Add("beamWarningMarkSize", beamWarningMarkSize)
			.Build();
	}

	void FromJson(const json& jsonData) override {
		Convert::fromJson(jsonData, "screenPos", screenPos);
		Convert::fromJson(jsonData, "worldZ", worldZ);
		Convert::fromJson(jsonData, "hp", hp);
		Convert::fromJson(jsonData, "hitSize", hitSize);

		Convert::fromJson(jsonData, "fireballSpawnOffsetY", fireballSpawnOffsetY);
		Convert::fromJson(jsonData, "bounceHeight", bounceHeight);
		Convert::fromJson(jsonData, "bounceSpeed", bounceSpeed);

		Convert::fromJson(jsonData, "beamCount", beamCount);
		Convert::fromJson(jsonData, "beamWarningTime", beamWarningTime);
		Convert::fromJson(jsonData, "beamTime", beamTime);
		Convert::fromJson(jsonData, "beamSpeed", beamSpeed);
		Convert::fromJson(jsonData, "beamStartX", beamStartX);
		Convert::fromJson(jsonData, "beamBlinkInterval", beamBlinkInterval);
		Convert::fromJson(jsonData, "beamSize", beamSize);
		Convert::fromJson(jsonData, "beamWarningLineSize", beamWarningLineSize);
		Convert::fromJson(jsonData, "beamWarningMarkSize", beamWarningMarkSize);

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
