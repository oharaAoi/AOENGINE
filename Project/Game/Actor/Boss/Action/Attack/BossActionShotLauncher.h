#pragma once
#include <memory>
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Engine/Module/Components/AI/ITaskNode.h"

class Boss;

/// <summary>
/// ランチャーを撃つ
/// </summary>
class BossActionShotLauncher :
	public ITaskNode<Boss> {
public:

	struct Parameter : public IJsonConverter {
		float bulletSpeed = 80.0f;
		float stiffenTime = 1.0f;
		float recoveryTime = 1.0f;

		Parameter() { SetName("BossActionShotLuncher"); }
		
		json ToJson(const std::string& id) const override {
			return JsonBuilder(id)
				.Add("bulletSpeed", bulletSpeed)
				.Add("stiffenTime", stiffenTime)
				.Add("recoveryTime", recoveryTime)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			fromJson(jsonData, "bulletSpeed", bulletSpeed);
			fromJson(jsonData, "stiffenTime", stiffenTime);
			fromJson(jsonData, "recoveryTime", recoveryTime);
		}

		void Debug_Gui() override;
	};

public:

	BossActionShotLauncher() = default;
	~BossActionShotLauncher() override = default;

	std::shared_ptr<IBehaviorNode> Clone() const override {
		return std::make_shared<BossActionShotLauncher>(*this);
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

	bool isFinish_;
	bool attackStart_;

};

