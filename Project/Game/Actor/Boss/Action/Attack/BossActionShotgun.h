#pragma once
#include <memory>
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Engine/System/AI/Node/BaseTaskNode.h"

class Boss;

/// <summary>
///ショットガン攻撃
/// </summary>
class BossActionShotgun :
	public AI::BaseTaskNode<Boss> {
public:

	struct Parameter : public AOENGINE::IJsonConverter {
		float recoveryTime = 0.2f;
		float bulletSpeed = 80.0f;
		float bulletSpread = 20.0f;
		float takeDamage = 10.0f;
		int kFireCount = 16;

		Parameter() { SetName("BossActionShotgun"); }
		
		json ToJson(const std::string& id) const override {
			return AOENGINE::JsonBuilder(id)
				.Add("recoveryTime", recoveryTime)
				.Add("bulletSpeed", bulletSpeed)
				.Add("bulletSpread", bulletSpread)
				.Add("takeDamage", takeDamage)
				.Add("kFireCount", kFireCount)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			Convert::fromJson(jsonData, "recoveryTime", recoveryTime);
			Convert::fromJson(jsonData, "bulletSpeed", bulletSpeed);
			Convert::fromJson(jsonData, "bulletSpread", bulletSpread);
			Convert::fromJson(jsonData, "takeDamage", takeDamage);
			Convert::fromJson(jsonData, "kFireCount", kFireCount);
		}

		void Debug_Gui() override;
	};

public:

	BossActionShotgun() = default;
	~BossActionShotgun() override = default;

	std::shared_ptr<AI::BaseBehaviorNode> Clone() const override {
		return std::make_shared<BossActionShotgun>(*this);
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

	// 攻撃
	void Shot();

private:

	Parameter param_;
	bool isFinishShot_;
	bool attackStart_;

};

