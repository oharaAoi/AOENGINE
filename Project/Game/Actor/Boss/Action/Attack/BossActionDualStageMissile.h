#pragma once
#include <memory>
#include "Engine/Lib/Math/Quaternion.h"
#include "Engine/System/AI/Node/ITaskNode.h"
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Engine/Utilities/Timer.h"

class Boss;

/// <summary>
/// 2弾式のミサイル
/// </summary>
class BossActionDualStageMissile :
	public ITaskNode<Boss> {
public: // データ構造体

	struct Parameter : public IJsonConverter {
		float weight = 0.5f;
		float recoveryTime = 1.0f;	// 攻撃後の硬直時間
		float bulletSpeed = 90.0f;	// 弾速
		float takeDamage = 30.0f;	// 与えるダメージ
		uint32_t fireNum = 14;		// 発射する数
		float firstSpeedRaito = 0.1f;	// 初期速度の割合
		float trakingRaito = 0.05f;		// 追従の割合

		float spreadAngleDeg = 60.0f;	// 拡散する角度
		float secondPhaseTime = 0.5f;	// 2発目までの時間
		float lineOffset = 1.0f;		// 2列間の距離

		Parameter() { SetName("BossActionDualStageMissile"); }

		json ToJson(const std::string& id) const override {
			return JsonBuilder(id)
				.Add("weight", weight)
				.Add("recoveryTime", recoveryTime)
				.Add("bulletSpeed", bulletSpeed)
				.Add("takeDamage", takeDamage)
				.Add("fireNum", fireNum)
				.Add("firstSpeedRaito", firstSpeedRaito)
				.Add("trakingRaito", trakingRaito)
				.Add("spreadAngleDeg", spreadAngleDeg)
				.Add("secondPhaseTime", secondPhaseTime)
				.Add("lineOffset", lineOffset)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			fromJson(jsonData, "weight", weight);
			fromJson(jsonData, "recoveryTime", recoveryTime);
			fromJson(jsonData, "bulletSpeed", bulletSpeed);
			fromJson(jsonData, "takeDamage", takeDamage);
			fromJson(jsonData, "fireNum", fireNum);
			fromJson(jsonData, "firstSpeedRaito", firstSpeedRaito);
			fromJson(jsonData, "trakingRaito", trakingRaito);
			fromJson(jsonData, "spreadAngleDeg", spreadAngleDeg);
			fromJson(jsonData, "secondPhaseTime", secondPhaseTime);
			fromJson(jsonData, "lineOffset", lineOffset);
		}

		void Debug_Gui() override;
	};

public: // コンストラクタ

	BossActionDualStageMissile() = default;
	~BossActionDualStageMissile() = default;

	std::shared_ptr<IBehaviorNode> Clone() const override {
		return std::make_shared<BossActionDualStageMissile>(*this);
	}

public:

	// 実行処理
	BehaviorStatus Execute() override;
	// weight値の計算
	float EvaluateWeight() override;
	// 編集処理
	void Debug_Gui() override;
	// 終了判定
	bool IsFinish() override;
	// Actionを実行できるかの確認
	bool CanExecute() override;
	// 初期化
	void Init() override;
	// 更新
	void Update() override;
	// 終了処理
	void End() override;

private:

	/// <summary>
	/// 発射
	/// </summary>
	void Shot();

	/// <summary>
	/// Playerの方向を向かせる処理
	/// </summary>
	void LookPlayer();

private:

	Parameter param_;

	// 弾を打ち終わったかのフラグ
	bool isFinishShot_ = false;

	// 2発目かどうか
	bool isSecondPhase_ = false;

	Timer secondPhaseTimer_;
};

