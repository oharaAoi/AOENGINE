#pragma once
#include <memory>
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Engine/Module/Components/AI/ITaskNode.h"

class Boss;

/// <summary>
/// ミサイルを放つ
/// </summary>
class BossActionShotMissile :
	public ITaskNode<Boss> {
public:

	struct Parameter : public IJsonConverter {
		float recoveryTime = 0.1f;
		float shotInterval = 0.1f;
		float bulletSpeed = 100.0f;

		Parameter() { SetName("bossActionShotMissile"); }

		json ToJson(const std::string& id) const override {
			return JsonBuilder(id)
				.Add("recoveryTime", recoveryTime)
				.Add("shotInterval", shotInterval)
				.Add("bulletSpeed", bulletSpeed)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			fromJson(jsonData, "recoveryTime", recoveryTime);
			fromJson(jsonData, "shotInterval", shotInterval);
			fromJson(jsonData, "bulletSpeed", bulletSpeed);
		}

		void Debug_Gui() override;
	};

public:

	BossActionShotMissile() = default;
	~BossActionShotMissile() override = default;

	std::shared_ptr<IBehaviorNode> Clone() const override {
		return std::make_shared<BossActionShotMissile>(*this);
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

private :
	Parameter param_;

	// 弾を打ち終わったかのフラグ
	bool isFinishShot_;

	bool attackStart_;

	const uint32_t kFireCount_ = 4;
	uint32_t fireCount_;

};