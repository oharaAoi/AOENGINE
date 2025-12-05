#pragma once
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Engine/System/AI/Node/ITaskNode.h"

class Boss;

/// <summary>
/// バリアを展開する
/// </summary>
class BossActionDeployArmor :
	public ITaskNode<Boss> {
public:

	struct Parameter : public IJsonConverter {
		float deployTime = 3.0f;
		float randShakeValue = 2.0f;

		Parameter() { SetName("bossActionDeployArmor"); }
		
		json ToJson(const std::string& id) const override {
			return JsonBuilder(id)
				.Add("deployTime", deployTime)
				.Add("randShakeValue", randShakeValue)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			fromJson(jsonData, "deployTime", deployTime);
			fromJson(jsonData, "randShakeValue", randShakeValue);
		}

		void Debug_Gui() override;
	};
public:

	BossActionDeployArmor();
	~BossActionDeployArmor() override = default;

	std::shared_ptr<BaseBehaviorNode> Clone() const override {
		return std::make_shared<BossActionDeployArmor>(*this);
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

	Parameter param_;
	bool isDeploy_ = false;

};

