#pragma once
#include <memory>
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Engine/Module/Components/AI/Node/ITaskNode.h"

class Boss;

/// <summary>
/// 連射攻撃
/// </summary>
class BossActionRapidfire :
	public ITaskNode<Boss> {
public:

	struct Parameter : public IJsonConverter {
		float shotInterval = 0.1f;
		float bulletSpeed = 80.0f;
		int kFireCount = 20;
		float recoveryTime = 0.5f;

		Parameter() { SetName("BossActionRapidfire"); }

		json ToJson(const std::string& id) const override {
			return JsonBuilder(id)
				.Add("shotInterval", shotInterval)
				.Add("bulletSpeed", bulletSpeed)
				.Add("kFireCount", kFireCount)
				.Add("recoveryTime", recoveryTime)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			fromJson(jsonData, "shotInterval", shotInterval);
			fromJson(jsonData, "bulletSpeed", bulletSpeed);
			fromJson(jsonData, "kFireCount", kFireCount);
			fromJson(jsonData, "recoveryTime", recoveryTime);
		}

		void Debug_Gui() override;
	};

public:

	BossActionRapidfire() = default;
	~BossActionRapidfire() override = default;

	std::shared_ptr<IBehaviorNode> Clone() const override {
		return std::make_shared<BossActionRapidfire>(*this);
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
	/// 攻撃
	/// </summary>
	void Shot();

	Parameter param_;

	bool isFinishShot_;
	int fireCount_;

	bool attackStart_;

};

