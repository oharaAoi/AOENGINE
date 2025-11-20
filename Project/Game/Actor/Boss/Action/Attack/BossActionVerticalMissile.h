#pragma once
#include "Engine/Module/Components/AI/Node/ITaskNode.h"
#include "Engine/Lib/Json/IJsonConverter.h"

class Boss;

/// <summary>
/// 垂直方向からミサイルを連続して放つ
/// </summary>
class BossActionVerticalMissile :
	public ITaskNode<Boss> {
public:

	struct Parameter : public IJsonConverter {
		float bulletSpeed = 80.0f;
		float fireRadius = 4.0f;
		int kFireCount = 10;

		Parameter() { SetName("BossActionVerticalMissile"); }

		json ToJson(const std::string& id) const override {
			return JsonBuilder(id)
				.Add("bulletSpeed", bulletSpeed)
				.Add("fireRadius", fireRadius)
				.Add("kFireCount", kFireCount)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			fromJson(jsonData, "bulletSpeed", bulletSpeed);
			fromJson(jsonData, "fireRadius", fireRadius);
			fromJson(jsonData, "kFireCount", kFireCount);
		}

		void Debug_Gui() override;
	};

public:

	BossActionVerticalMissile() = default;
	~BossActionVerticalMissile() override = default;

	std::shared_ptr<IBehaviorNode> Clone() const override {
		return std::make_shared<BossActionVerticalMissile>(*this);
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
	int fireCount_;

	float angle_;

	bool attackStart_;

};

