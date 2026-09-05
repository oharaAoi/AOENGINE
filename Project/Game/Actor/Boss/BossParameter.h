#pragma once
#include <array>
#include <cstddef>
#include <cstdint>
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Engine/System/Editor/Parameter/CustomParameter.h"
#include "Engine/Lib/Math/Vector2.h"
#include "Engine/Lib/Math/Vector3.h"
#include "Engine/Module/Entity/Camera/Component/CameraShakeParameters.h"

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
	float fireballDamage = 1.0f;		// 火球が当たった時のダメージ
	float fireballSpawnHeight = 12.0f;	// プレイヤーの何ユニット上から落とすか
	float bounceHeight = 1.0f;			// 跳ねる高さ
	float bounceSpeed = 8.0f;			// 跳ねる速さ

	// --- 攻撃2: ビーム ---
	float beamDamage = 1.0f;				// ビームが当たった時のダメージ
	int32_t beamCount = 3;					// ビーム回数
	float beamWarningTime = 1.5f;			// 予測線秒数
	float beamTime = 1.0f;					// ビーム秒数
	float beamSpeed = 40.0f;				// ビームが横切る速さ
	float beamStartX = -30.0f;				// ビームを出す画面外のX
	float beamBlinkInterval = 0.1f;			// 予測線の点滅間隔
	Math::Vector3 beamSize{ 40.0f, 1.2f, 1.0f };		// ビームの大きさ
	Math::Vector3 beamWarningLineSize{ 40.0f, 1.2f, 1.0f };	// 予測線の大きさ
	Math::Vector3 beamWarningMarkSize{ 1.5f, 1.5f, 1.0f };	// 危険マークの大きさ

	// --- 攻撃3: 足止め落とし ---
	int32_t stopperCount = 3;			// 足止めの個数
	float stopperSpeed = 12.0f;			// 落下速度
	float stopperLifeTime = 5.0f;		// 着地してから消えるまでの秒数
	float stopperSpawnHeight = 12.0f;	// プレイヤーの何ユニット上から落とすか
	Math::Vector3 stopperSize{ 1.0f, 1.0f, 1.0f };	// 足止めの大きさ
	int32_t stopperEaseKind = 1;			// 落下のイージング種類

	// 着地時のカメラシェイク。揺れ方の詳細はこのリクエスト側で調整する
	CameraShakeRequest stopperLandShake;

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
		AddParameter("Fireball Damage", fireballDamage, 0.1f, 0.0f, 1000.0f);
		AddParameter("Fireball Spawn Height", fireballSpawnHeight, 0.1f, 0.0f, 1000.0f);
		AddParameter("Bounce Height", bounceHeight, 0.1f, 0.0f, 100.0f);
		AddParameter("Bounce Speed", bounceSpeed, 0.1f, 0.0f, 100.0f);

		AddSeparatorText("Attack2: Beam");
		AddParameter("Beam Damage", beamDamage, 0.1f, 0.0f, 1000.0f);
		AddParameter("Beam Count", beamCount, 1.0f, 0.0f, 100.0f);
		AddParameter("Beam Warning Time", beamWarningTime, 0.01f, 0.0f, 10.0f);
		AddParameter("Beam Time", beamTime, 0.01f, 0.0f, 10.0f);
		AddParameter("Beam Speed", beamSpeed, 0.1f, 0.0f, 1000.0f);
		AddParameter("Beam Start X", beamStartX, 0.1f, -1000.0f, 1000.0f);
		AddParameter("Beam Blink Interval", beamBlinkInterval, 0.01f, 0.0f, 5.0f);
		AddParameter("Beam Size", beamSize, 0.1f);
		AddParameter("Warning Line Size", beamWarningLineSize, 0.1f);
		AddParameter("Warning Mark Size", beamWarningMarkSize, 0.1f);

		AddSeparatorText("Attack3: Stopper");
		AddParameter("Stopper Count", stopperCount, 1.0f, 0.0f, 100.0f);
		AddParameter("Stopper Speed", stopperSpeed, 0.1f, 0.0f, 1000.0f);
		AddParameter("Stopper Life Time", stopperLifeTime, 0.1f, 0.0f, 100.0f);
		AddParameter("Stopper Spawn Height", stopperSpawnHeight, 0.1f, 0.0f, 1000.0f);
		AddParameter("Stopper Size", stopperSize, 0.1f);
		AddParameter("Stopper Ease Kind", stopperEaseKind, 1.0f, 0.0f, 30.0f);

		
		stopperLandShake.SetGroupName("Boss");
		stopperLandShake.SetName("stopperLandShake");
	}

	json ToJson(const std::string& id) const override {
		return AOENGINE::JsonBuilder(id)
			.Add("screenPos", screenPos)
			.Add("worldZ", worldZ)
			.Add("hp", hp)
			.Add("hitSize", hitSize)
			.Add("phaseSwitchRatio", json(phaseSwitchRatio))
			.Add("fireballDamage", fireballDamage)
			.Add("fireballSpawnHeight", fireballSpawnHeight)
			.Add("bounceHeight", bounceHeight)
			.Add("bounceSpeed", bounceSpeed)
			.Add("beamDamage", beamDamage)
			.Add("beamCount", beamCount)
			.Add("beamWarningTime", beamWarningTime)
			.Add("beamTime", beamTime)
			.Add("beamSpeed", beamSpeed)
			.Add("beamStartX", beamStartX)
			.Add("beamBlinkInterval", beamBlinkInterval)
			.Add("beamSize", beamSize)
			.Add("beamWarningLineSize", beamWarningLineSize)
			.Add("beamWarningMarkSize", beamWarningMarkSize)
			.Add("stopperCount", stopperCount)
			.Add("stopperSpeed", stopperSpeed)
			.Add("stopperLifeTime", stopperLifeTime)
			.Add("stopperSpawnHeight", stopperSpawnHeight)
			.Add("stopperSize", stopperSize)
			.Add("stopperEaseKind", stopperEaseKind)
			.Build();
	}

	void FromJson(const json& jsonData) override {
		Convert::fromJson(jsonData, "screenPos", screenPos);
		Convert::fromJson(jsonData, "worldZ", worldZ);
		Convert::fromJson(jsonData, "hp", hp);
		Convert::fromJson(jsonData, "hitSize", hitSize);

		Convert::fromJson(jsonData, "fireballDamage", fireballDamage);
		Convert::fromJson(jsonData, "fireballSpawnHeight", fireballSpawnHeight);
		Convert::fromJson(jsonData, "bounceHeight", bounceHeight);
		Convert::fromJson(jsonData, "bounceSpeed", bounceSpeed);

		Convert::fromJson(jsonData, "beamDamage", beamDamage);
		Convert::fromJson(jsonData, "beamCount", beamCount);
		Convert::fromJson(jsonData, "beamWarningTime", beamWarningTime);
		Convert::fromJson(jsonData, "beamTime", beamTime);
		Convert::fromJson(jsonData, "beamSpeed", beamSpeed);
		Convert::fromJson(jsonData, "beamStartX", beamStartX);
		Convert::fromJson(jsonData, "beamBlinkInterval", beamBlinkInterval);
		Convert::fromJson(jsonData, "beamSize", beamSize);
		Convert::fromJson(jsonData, "beamWarningLineSize", beamWarningLineSize);
		Convert::fromJson(jsonData, "beamWarningMarkSize", beamWarningMarkSize);

		Convert::fromJson(jsonData, "stopperCount", stopperCount);
		Convert::fromJson(jsonData, "stopperSpeed", stopperSpeed);
		Convert::fromJson(jsonData, "stopperLifeTime", stopperLifeTime);
		Convert::fromJson(jsonData, "stopperSpawnHeight", stopperSpawnHeight);
		Convert::fromJson(jsonData, "stopperSize", stopperSize);
		Convert::fromJson(jsonData, "stopperEaseKind", stopperEaseKind);

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
