#pragma once
#include "Engine/Module/Components/AI/ITaskNode.h"

class Boss;

/// <summary>
/// プレイヤーと高さをあわせる
/// </summary>
class BossActionAdjustHeight :
	public ITaskNode<Boss> {
public:

	struct Parameter : public IJsonConverter {
		float smoothTime = 0.1f;	// 追従の速度
		float maxSpeed = 10 ^ 8;	// 追従の最大速度

		Parameter() { SetName("BossActionAdjustHeight"); }

		json ToJson(const std::string& id) const override {
			return JsonBuilder(id)
				.Add("smoothTime", smoothTime)
				.Add("maxSpeed", maxSpeed)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			fromJson(jsonData, "smoothTime", smoothTime);
			fromJson(jsonData, "maxSpeed", maxSpeed);
		}

		void Debug_Gui() override;
	};

public:

	BossActionAdjustHeight() = default;
	~BossActionAdjustHeight() override = default;

	std::shared_ptr<IBehaviorNode> Clone() const override {
		return std::make_shared<BossActionAdjustHeight>(*this);
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
	float speed_;

	float distance_;

};

