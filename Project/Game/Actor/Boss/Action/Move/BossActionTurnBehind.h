#pragma once
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Engine/System/AI/Node/BaseTaskNode.h"
#include "Engine/Module/Components/Physics/Rigidbody.h"
#include "Engine/Lib/Math/Curve.h"

class Boss;

/// <summary>
/// 敵の背後に回る処理
/// </summary>
class BossActionTurnBehind :
	public AI::BaseTaskNode<Boss> {
public: // データ構造体

	struct Parameter : public AOENGINE::IJsonConverter {
		float moveSpeed = 40.f;
		float moveTime = 1.5f;
		Math::Curve moveCurve;

		Parameter() {
			SetGroupName("BossAction");
			SetName("BossActionTurnBehind");
		}

		json ToJson(const std::string& id) const override {
			json curveJson = moveCurve.ToJson();
			return AOENGINE::JsonBuilder(id)
				.Add("moveSpeed", moveSpeed)
				.Add("moveTime", moveTime)
				.Add("moveCurve", curveJson)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			fromJson(jsonData, "moveSpeed", moveSpeed);
			fromJson(jsonData, "moveTime", moveTime);
			fromJson(jsonData, "moveCurve", moveCurve);
		}

		void Debug_Gui() override;
	};

public: // コンストラクタ

	BossActionTurnBehind() = default;
	~BossActionTurnBehind() = default;

	std::shared_ptr<AI::BaseBehaviorNode> Clone() const override {
		return std::make_shared<BossActionTurnBehind>(*this);
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

	void Move();

private:

	AOENGINE::Rigidbody* pRigidBody_;

	Parameter param_;
	
	Math::Vector3 velocity_;
	Math::Vector3 acceleration_;

	Math::Vector3 toCenter_;
};

