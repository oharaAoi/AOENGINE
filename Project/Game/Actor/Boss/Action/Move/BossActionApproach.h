#pragma once
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Engine/System/AI/Node/ITaskNode.h"

class Boss;

/// <summary>
/// Bossの近づくアクション
/// </summary>
class BossActionApproach :
	public ITaskNode<Boss> {
public:

	struct Parameter : public IJsonConverter {
		float moveSpeed = 20.f;			// 移動速度
		float moveTime = 2.0f;			// 移動時間
		float deceleration = 2.f;		// 減速率
		float maxSpinDistance = 0.8f;	// 旋回量
		float quitApproachLength = 5.f;	// どのくらいまで近づくか
		float decayRate = 5.f;			// 減衰率
		float recoveryTime = 0.0f;		// 硬直時間
		float rotateT = 0.1f;

		Parameter() { SetName("bossActionApproach"); }

		json ToJson(const std::string& id) const override {
			return JsonBuilder(id)
				.Add("moveSpeed", moveSpeed)
				.Add("moveTime", moveTime)
				.Add("deceleration", deceleration)
				.Add("maxSpinDistance", maxSpinDistance)
				.Add("quitApproachLength", quitApproachLength)
				.Add("decayRate", decayRate)
				.Add("recoveryTime", recoveryTime)
				.Add("rotateT", rotateT)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			fromJson(jsonData, "moveSpeed", moveSpeed);
			fromJson(jsonData, "moveTime", moveTime);
			fromJson(jsonData, "deceleration", deceleration);
			fromJson(jsonData, "maxSpinDistance", maxSpinDistance);
			fromJson(jsonData, "quitApproachLength", quitApproachLength);
			fromJson(jsonData, "decayRate", decayRate);
			fromJson(jsonData, "recoveryTime", recoveryTime);
			fromJson(jsonData, "rotateT", rotateT);
		}

		void Debug_Gui() override;
	};

public:

	BossActionApproach() = default;
	~BossActionApproach() = default;

	std::shared_ptr<IBehaviorNode> Clone() const override {
		return std::make_shared<BossActionApproach>(*this);
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

	// 旋回しながら敵に近づく
	void SpinApproach();

	// 敵に近づいて来る
	void Approach();

private :

	Parameter param_;
	Parameter initParam_;

	bool isShot_;

	// 近づく挙動に必要な変数
	Vector3 toPlayer_;
	float distance_;
	Vector3 direToPlayer_;

	Vector3 lateral_; // player方向に対する垂直なベクトル

	float spinAmount_;	// 旋回量
	Vector3 offsetDire_;

	bool stopping_;

};