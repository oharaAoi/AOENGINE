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

	Math::Vector3 baseScale{ 0.6f, 0.6f, 0.6f };
	Math::Vector3 hitSize{ 2.0f, 2.0f, 2.0f };

	// フェーズが切り替わる残HPの割合。高い順に並べる
	std::array<float, kPhaseSwitchCount> phaseSwitchRatio;

	// 各攻撃が解禁されるフェーズ
	int32_t fallFireUnlockPhase = 0;
	int32_t beamUnlockPhase = 1;
	int32_t stopperUnlockPhase = 2;

	// フェーズが上がった時の演出
	float phaseChangeTime = 1.2f;		// 切り替え演出の長さ
	float phaseChangeScaleRate = 0.25f;	// どれだけ膨らむか(基準スケールへの倍率)
	CameraShakeRequest phaseChangeShake;

	// --- 攻撃1: 火球落とし ---
	float fireballDamage;		// 火球が当たった時のダメージ
	float fireballStartDelay = 0.5f;	// アニメーション再生から落とし始めるまでの秒数
	int32_t fireballDropCount;		// 1回の行動で落とす個数
	float fireballDropInterval;	// 次の1個を落とすまでの間隔
	float fireballSpawnHeight;	// プレイヤーの何ユニット上から落とすか
	float fireballFallStartSpeed;	// 落ち始めの速度
	float fireballMaxFallSpeed;		// 落下速度の上限
	float fireballFallSpeedUpTime;	// 上限の速度に達するまでの時間
	// 速度の補間に使うイージング種類
	int32_t fireballFallEaseKind = 1;

	// --- 攻撃2: ビーム ---
	float beamDamage;				// ビームが当たった時のダメージ
	float beamStartDelay = 0.5f;			// アニメーション再生から予測線を出すまでの秒数
	int32_t beamCount;					// ビーム回数
	float beamWarningTime;			// 予測線秒数
	float beamTime;					// ビーム秒数
	float beamSpeed;				// ビームが横切る速さ
	float beamStartX;				// ビームを出す画面外のX
	float beamBlinkInterval;			// 予測線の点滅間隔
	Math::Vector3 beamSize;		// ビームの大きさ
	Math::Vector3 beamWarningLineSize;	// 予測線の大きさ
	Math::Vector3 beamWarningMarkSize;	// 危険マークの大きさ

	// --- 攻撃3: 足止め落とし ---
	int32_t stopperCount;			// 足止めの個数
	float stopperStartDelay = 0.5f;		// アニメーション再生から落とし始めるまでの秒数
	float stopperSpeed;			// 落下速度
	float stopperLifeTime;		// 着地してから消えるまでの秒数
	float stopperSpawnHeight;	// プレイヤーの何ユニット上から落とすか
	Math::Vector3 stopperSize;	// 足止めの大きさ
	// 落下のイージング種類
	int32_t stopperEaseKind = 1;

	// 着地時のカメラシェイク
	CameraShakeRequest stopperLandShake;

	// --- アニメーション ---
	float animationBlendSpeed = 0.2f;	// 切り替える時の補間速度
	float animationSpeed = 1.0f;		// 再生速度

	// --- 待機 ---
	float idleTimeMin = 1.0f;	// 次の攻撃までの最短時間
	float idleTimeMax = 2.5f;	// 次の攻撃までの最長時間

	// --- 撃破 ---
	float defeatHideTime = 1.5f;	// 撃破されてから姿を消すまでの秒数

	BossParameter() : CustomParameterSet("Boss") {
		SetGroupName("Boss");
		SetName("bossParameter");

		AddSeparatorText("Boss");
		AddParameter("Screen Pos(px)", screenPos, 1.0f);
		AddParameter("World Z", worldZ, 0.1f);
		AddParameter("Max HP", hp, 1.0f, 0.0f, 100000.0f);
		AddParameter("Base Scale", baseScale, 0.01f);
		AddParameter("Hit Size", hitSize, 0.1f);

		AddSeparatorText("Phase");
		AddParameter("Phase Switch Ratio 0", phaseSwitchRatio[0], 0.01f, 0.0f, 1.0f);
		AddParameter("Phase Switch Ratio 1", phaseSwitchRatio[1], 0.01f, 0.0f, 1.0f);
		AddParameter("Unlock Phase: FallFire", fallFireUnlockPhase, 1.0f, 0.0f, 10.0f);
		AddParameter("Unlock Phase: Beam", beamUnlockPhase, 1.0f, 0.0f, 10.0f);
		AddParameter("Unlock Phase: Stopper", stopperUnlockPhase, 1.0f, 0.0f, 10.0f);
		AddParameter("Phase Change Time", phaseChangeTime, 0.01f, 0.0f, 60.0f);
		AddParameter("Phase Change Scale Rate", phaseChangeScaleRate, 0.01f, 0.0f, 10.0f);

		AddSeparatorText("Attack1: FallFire");
		AddParameter("Fireball Damage", fireballDamage, 0.1f, 0.0f, 1000.0f);
		AddParameter("Fireball Start Delay", fireballStartDelay, 0.01f, 0.0f, 60.0f);
		AddParameter("Fireball Drop Count", fireballDropCount, 1.0f, 0.0f, 100.0f);
		AddParameter("Fireball Drop Interval", fireballDropInterval, 0.01f, 0.0f, 60.0f);
		AddParameter("Fireball Spawn Height", fireballSpawnHeight, 0.1f, 0.0f, 1000.0f);
		AddParameter("Fireball Fall Start Speed", fireballFallStartSpeed, 0.1f, 0.0f, 1000.0f);
		AddParameter("Fireball Max Fall Speed", fireballMaxFallSpeed, 0.1f, 0.0f, 1000.0f);
		AddParameter("Fireball Fall SpeedUp Time", fireballFallSpeedUpTime, 0.01f, 0.0f, 60.0f);

		AddSeparatorText("Attack2: Beam");
		AddParameter("Beam Damage", beamDamage, 0.1f, 0.0f, 1000.0f);
		AddParameter("Beam Start Delay", beamStartDelay, 0.01f, 0.0f, 60.0f);
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
		AddParameter("Stopper Start Delay", stopperStartDelay, 0.01f, 0.0f, 60.0f);
		AddParameter("Stopper Speed", stopperSpeed, 0.1f, 0.0f, 1000.0f);
		AddParameter("Stopper Life Time", stopperLifeTime, 0.1f, 0.0f, 100.0f);
		AddParameter("Stopper Spawn Height", stopperSpawnHeight, 0.1f, 0.0f, 1000.0f);
		AddParameter("Stopper Size", stopperSize, 0.1f);

		AddSeparatorText("Animation");
		AddParameter("Animation Blend Speed", animationBlendSpeed, 0.01f, 0.0f, 10.0f);
		AddParameter("Animation Speed", animationSpeed, 0.01f, 0.0f, 10.0f);

		AddSeparatorText("Idle");
		AddParameter("Idle Time Min", idleTimeMin, 0.01f, 0.0f, 60.0f);
		AddParameter("Idle Time Max", idleTimeMax, 0.01f, 0.0f, 60.0f);

		AddSeparatorText("Defeat");
		AddParameter("Defeat Hide Time", defeatHideTime, 0.01f, 0.0f, 60.0f);


		stopperLandShake.SetGroupName("Boss");
		stopperLandShake.SetName("stopperLandShake");
		phaseChangeShake.SetGroupName("Boss");
		phaseChangeShake.SetName("phaseChangeShake");
	}

	json ToJson(const std::string& id) const override {
		return AOENGINE::JsonBuilder(id)
			.Add("screenPos", screenPos)
			.Add("worldZ", worldZ)
			.Add("hp", hp)
			.Add("baseScale", baseScale)
			.Add("hitSize", hitSize)
			.Add("phaseSwitchRatio", json(phaseSwitchRatio))
			.Add("fallFireUnlockPhase", fallFireUnlockPhase)
			.Add("beamUnlockPhase", beamUnlockPhase)
			.Add("stopperUnlockPhase", stopperUnlockPhase)
			.Add("phaseChangeTime", phaseChangeTime)
			.Add("phaseChangeScaleRate", phaseChangeScaleRate)
			.Add("fireballDamage", fireballDamage)
			.Add("fireballStartDelay", fireballStartDelay)
			.Add("fireballDropCount", fireballDropCount)
			.Add("fireballDropInterval", fireballDropInterval)
			.Add("fireballSpawnHeight", fireballSpawnHeight)
			.Add("fireballFallStartSpeed", fireballFallStartSpeed)
			.Add("fireballMaxFallSpeed", fireballMaxFallSpeed)
			.Add("fireballFallSpeedUpTime", fireballFallSpeedUpTime)
			.Add("fireballFallEaseKind", fireballFallEaseKind)
			.Add("beamDamage", beamDamage)
			.Add("beamStartDelay", beamStartDelay)
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
			.Add("stopperStartDelay", stopperStartDelay)
			.Add("stopperSpeed", stopperSpeed)
			.Add("stopperLifeTime", stopperLifeTime)
			.Add("stopperSpawnHeight", stopperSpawnHeight)
			.Add("stopperSize", stopperSize)
			.Add("stopperEaseKind", stopperEaseKind)
			.Add("animationBlendSpeed", animationBlendSpeed)
			.Add("animationSpeed", animationSpeed)
			.Add("idleTimeMin", idleTimeMin)
			.Add("idleTimeMax", idleTimeMax)
			.Add("defeatHideTime", defeatHideTime)
			.Build();
	}

	void FromJson(const json& jsonData) override {
		Convert::fromJson(jsonData, "screenPos", screenPos);
		Convert::fromJson(jsonData, "worldZ", worldZ);
		Convert::fromJson(jsonData, "hp", hp);
		Convert::fromJson(jsonData, "baseScale", baseScale);
		Convert::fromJson(jsonData, "hitSize", hitSize);

		Convert::fromJson(jsonData, "fallFireUnlockPhase", fallFireUnlockPhase);
		Convert::fromJson(jsonData, "beamUnlockPhase", beamUnlockPhase);
		Convert::fromJson(jsonData, "stopperUnlockPhase", stopperUnlockPhase);
		Convert::fromJson(jsonData, "phaseChangeTime", phaseChangeTime);
		Convert::fromJson(jsonData, "phaseChangeScaleRate", phaseChangeScaleRate);

		Convert::fromJson(jsonData, "fireballDamage", fireballDamage);
		Convert::fromJson(jsonData, "fireballStartDelay", fireballStartDelay);
		Convert::fromJson(jsonData, "fireballDropCount", fireballDropCount);
		Convert::fromJson(jsonData, "fireballDropInterval", fireballDropInterval);
		Convert::fromJson(jsonData, "fireballSpawnHeight", fireballSpawnHeight);
		Convert::fromJson(jsonData, "fireballFallStartSpeed", fireballFallStartSpeed);
		Convert::fromJson(jsonData, "fireballMaxFallSpeed", fireballMaxFallSpeed);
		Convert::fromJson(jsonData, "fireballFallSpeedUpTime", fireballFallSpeedUpTime);
		Convert::fromJson(jsonData, "fireballFallEaseKind", fireballFallEaseKind);

		Convert::fromJson(jsonData, "beamDamage", beamDamage);
		Convert::fromJson(jsonData, "beamStartDelay", beamStartDelay);
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
		Convert::fromJson(jsonData, "stopperStartDelay", stopperStartDelay);
		Convert::fromJson(jsonData, "stopperSpeed", stopperSpeed);
		Convert::fromJson(jsonData, "stopperLifeTime", stopperLifeTime);
		Convert::fromJson(jsonData, "stopperSpawnHeight", stopperSpawnHeight);
		Convert::fromJson(jsonData, "stopperSize", stopperSize);
		Convert::fromJson(jsonData, "stopperEaseKind", stopperEaseKind);

		Convert::fromJson(jsonData, "animationBlendSpeed", animationBlendSpeed);
		Convert::fromJson(jsonData, "animationSpeed", animationSpeed);

		Convert::fromJson(jsonData, "idleTimeMin", idleTimeMin);
		Convert::fromJson(jsonData, "idleTimeMax", idleTimeMax);
		Convert::fromJson(jsonData, "defeatHideTime", defeatHideTime);

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
