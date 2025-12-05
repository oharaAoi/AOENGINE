#pragma once
#include <memory>
#include <functional>
#include "Engine/Lib/Math/Quaternion.h"
#include "Engine/System/AI/Node/BaseTaskNode.h"
#include "Engine/Lib/Json/IJsonConverter.h"

class Boss;

/// <summary>
/// 全方位にミサイルを飛ばすアクション
/// </summary>
class BossActionAllRangeMissile :
	public BaseTaskNode<Boss> {
public:

	struct Parameter : public IJsonConverter {
		float recoveryTime = 1.0f;		// 攻撃後の硬直時間
		float bulletSpeed = 90.0f;		// 弾速
		float takeDamage = 30.0f;		// 与えるダメージ
		uint32_t fireNum = 9;			// 発射する数
		float firstSpeedRaito = 0.1f;	// 初期速度の割合
		float trakingRaito = 0.05f;		// 追従の割合
		
		Parameter() { SetName("BossActionAllRangeMissile"); }

		json ToJson(const std::string& id) const override {
			return JsonBuilder(id)
				.Add("recoveryTime", recoveryTime)
				.Add("bulletSpeed", bulletSpeed)
				.Add("takeDamage", takeDamage)
				.Add("fireNum", fireNum)
				.Add("firstSpeedRaito", firstSpeedRaito)
				.Add("trakingRaito", trakingRaito)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			fromJson(jsonData, "recoveryTime", recoveryTime);
			fromJson(jsonData, "bulletSpeed", bulletSpeed);
			fromJson(jsonData, "takeDamage", takeDamage);
			fromJson(jsonData, "fireNum", fireNum);
			fromJson(jsonData, "firstSpeedRaito", firstSpeedRaito);
			fromJson(jsonData, "trakingRaito", trakingRaito);
		}

		void Debug_Gui() override;
	};

public:

	BossActionAllRangeMissile() = default;
	~BossActionAllRangeMissile() override = default;

	std::shared_ptr<BaseBehaviorNode> Clone() const override {
		return std::make_shared<BossActionAllRangeMissile>(*this);
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

private :

	/// <summary>
	/// 発射
	/// </summary>
	void Shot();

	/// <summary>
	/// Playerの方向を向かせる処理
	/// </summary>
	void LookPlayer();

private :

	Parameter param_;

	Quaternion playerToRotation_;

	// LookPlayerに関する変数
	float lookTime_ = 1.f;

	// 弾を打ち終わったかのフラグ
	bool isFinishShot_;

	std::function<void()> mainAction_;

};

