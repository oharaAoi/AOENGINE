#pragma once
#include <memory>
#include <functional>
#include "Engine/Lib/Math/Quaternion.h"
#include "Engine/Module/Components/AI/ITaskNode.h"
#include "Engine/Lib/Json/IJsonConverter.h"

class Boss;

/// <summary>
/// 全方位にミサイルを飛ばすアクション
/// </summary>
class BossActionAllRangeMissile :
	public ITaskNode<Boss> {
public:

	struct Parameter : public IJsonConverter {
		float coolTime = 30.0f;
		float recoveryTime = 1.0f;	// 攻撃後の硬直時間
		
		Parameter() { SetName("BossActionAllRangeMissile"); }

		json ToJson(const std::string& id) const override {
			return JsonBuilder(id)
				.Add("coolTime", coolTime)
				.Add("recoveryTime", recoveryTime)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			fromJson(jsonData, "coolTime", coolTime);
			fromJson(jsonData, "recoveryTime", recoveryTime);
		}

		void Debug_Gui() override;
	};

public:

	BossActionAllRangeMissile() = default;
	~BossActionAllRangeMissile() override = default;

	std::shared_ptr<IBehaviorNode> Clone() const override {
		return std::make_shared<BossActionAllRangeMissile>(*this);
	}

	BehaviorStatus Execute() override;

	float EvaluateWeight() override;

	void Debug_Gui() override;

	bool IsFinish() override;
	bool CanExecute() override;

	void Init() override;
	void Update() override;
	void End() override;

private :

	/// <summary>
	/// 発射
	/// </summary>
	void Shot();

	/// <summary>
	/// Playerの方向を向かせる処理
	/// </summary>
	void LookPlayer();

private :

	Parameter param_;

	Quaternion playerToRotation_;

	float bulletSpeed_;

	// LookPlayerに関する変数
	float lookTime_ = 1.f;

	// 弾を打ち終わったかのフラグ
	bool isFinishShot_;

	std::function<void()> mainAction_;

};

