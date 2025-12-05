#pragma once
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Engine/System/AI/Node/BaseTaskNode.h"

class Boss;

/// <summary>
/// Bossの距離を取る行動
/// </summary>
class BossActionKeepDistance :
	public BaseTaskNode<Boss> {
public:

	struct Parameter : public AOENGINE::IJsonConverter {
		float moveSpeed = 60.f;
		float moveTime = 2.0f;
		float getDistance = 10.0f;
		float decayRate = 4.0f;
		float rotateT = 0.1f;

		Parameter() { SetName("bossKeepDistance"); }

		json ToJson(const std::string& id) const override {
			return AOENGINE::JsonBuilder(id)
				.Add("moveSpeed", moveSpeed)
				.Add("moveTime", moveTime)
				.Add("getDistance", getDistance)
				.Add("decayRate", decayRate)
				.Add("rotateT", rotateT)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			fromJson(jsonData, "moveSpeed", moveSpeed);
			fromJson(jsonData, "moveTime", moveTime);
			fromJson(jsonData, "getDistance", getDistance);
			fromJson(jsonData, "decayRate", decayRate);
			fromJson(jsonData, "rotateT", rotateT);
		}

		void Debug_Gui() override;
	};

public:

	BossActionKeepDistance() = default;
	~BossActionKeepDistance() = default;

	std::shared_ptr<BaseBehaviorNode> Clone() const override {
		return std::make_shared<BossActionKeepDistance>(*this);
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

public:

	// 直線で離れる
	void Direct();

	// 旋回しながら離れる
	void Spin();

	// 止める
	void Stop();

private:

	Parameter param_;

	Vector3 velocity_;
	Vector3 accel_;

	int moveType_;

	// 旋回に必要な座標
	Vector3 centerPos_;

	bool stopping_;

};

