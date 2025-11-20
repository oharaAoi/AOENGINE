#pragma once
#include "Engine/Module/Components/AI/Node/ITaskNode.h"
#include "Engine/Lib/Json/IJsonConverter.h"

class Boss;


class BossActionFloat :
	public ITaskNode<Boss>{

public:

	struct Parameter : public IJsonConverter {
		float moveTime = 0.5f;		// 移動時間
		float moveSpeed = 10.0f;	// 移動速度

		Parameter() { SetName("BossActionFloat"); }

		json ToJson(const std::string& id) const override {
			return JsonBuilder(id)
				.Add("moveTime", moveTime)
				.Add("moveSpeed", moveSpeed)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			fromJson(jsonData, "moveTime", moveTime);
			fromJson(jsonData, "moveSpeed", moveSpeed);
		}

		void Debug_Gui() override;
	};

public:

	BossActionFloat() = default;
	~BossActionFloat() override = default;

	std::shared_ptr<IBehaviorNode> Clone() const override {
		return std::make_shared<BossActionFloat>(*this);
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
};

