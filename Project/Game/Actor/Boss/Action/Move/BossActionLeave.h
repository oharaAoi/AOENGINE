#pragma once
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Engine/System/AI/Node/BaseTaskNode.h"

class Boss;

/// <summary>
/// Bossの離れるアクション
/// </summary>
class BossActionLeave :
	public BaseTaskNode<Boss> {
public:	// 構造体

	struct Parameter : public AOENGINE::IJsonConverter {
		float moveSpeed = 40.f;
		float moveTime = 1.5f;
		float decayRate = 4.0f;
		float finishDistance = 1.0f;
		float rotateT = 0.1f;

		Parameter() { SetName("bossActionLeave"); }

		json ToJson(const std::string& id) const override {
			return AOENGINE::JsonBuilder(id)
				.Add("moveSpeed", moveSpeed)
				.Add("moveTime", moveTime)
				.Add("decayRate", decayRate)
				.Add("finishDistance", finishDistance)
				.Add("rotateT", rotateT)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			fromJson(jsonData, "moveSpeed", moveSpeed);
			fromJson(jsonData, "moveTime", moveTime);
			fromJson(jsonData, "decayRate", decayRate);
			fromJson(jsonData, "finishDistance", finishDistance);
			fromJson(jsonData, "rotateT", rotateT);
		}

		void Debug_Gui() override;
	};

public:

	BossActionLeave() = default;
	~BossActionLeave() = default;

	std::shared_ptr<BaseBehaviorNode> Clone() const override {
		return std::make_shared<BossActionLeave>(*this);
	}

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

	// 離れるアクション
	void Leave();

	// 止まるアクション
	void Stop();

private:

	Parameter param_;

	Vector3 velocity_;
	Vector3 accel_;

	bool stopping_;

};