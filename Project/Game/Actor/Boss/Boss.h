#pragma once
#include <memory>
// Engine
#include "Engine/Module/Components/GameObject/BaseEntity.h"
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Engine/System/AI/BehaviorTree.h"
// Game
#include "Game/State/StateMachine.h"
#include "Game/Actor/Boss/Bullet/BossBulletManager.h"
#include "Game/Actor/Enemy/BaseEnemy.h"
#include "Game/Actor/Weapon/Armors.h"
#include "Game/Camera/FollowCamera.h"
#include "Game/Effects/AttackArmor.h"

/// <summary>
/// ボスのフェーズ
/// </summary>
enum class BossPhase {
	First,
	Second,
};

/// <summary>
/// ボスの戦術
/// </summary>
enum class ActionStrategy {
	Aggressive,
	Defensive
};

class BossUIs;

/// <summary>
/// Bossクラス
/// </summary>
class Boss :
	public BaseEnemy {
public:

	struct Parameter : public AOENGINE::IJsonConverter {
		float armorCoolTime = 20.0f;
		float attackArmorDamage = 100.0f;
		float angularVelocity = 90.f; // 角速度
		float angularThreshold = 10.f; // 角速度
		float idealDistance = 10.f; // 理想距離
		float maxDistance = 10.f; // 最大距離
		float aggressionBaseScore = 0.5f; // 積極性のベース値

		std::string worldStatePath = "";		// worldStateのパス
		Math::Vector2 treeStateOffset;

		Parameter() { SetName("bossParameter"); }

		json ToJson(const std::string& id) const override {
			return AOENGINE::JsonBuilder(id)
				.Add("armorCoolTime", armorCoolTime)
				.Add("angularVelocity", angularVelocity)
				.Add("angularThreshold", angularThreshold)
				.Add("worldStatePath", worldStatePath)
				.Add("idealDistance", idealDistance)
				.Add("maxDistance", maxDistance)
				.Add("aggressionBaseScore", aggressionBaseScore)
				.Add("treeStateOffset", treeStateOffset)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			Convert::fromJson(jsonData, "armorCoolTime", armorCoolTime);
			Convert::fromJson(jsonData, "angularVelocity", angularVelocity);
			Convert::fromJson(jsonData, "angularThreshold", angularThreshold);
			Convert::fromJson(jsonData, "worldStatePath", worldStatePath);
			Convert::fromJson(jsonData, "idealDistance", idealDistance);
			Convert::fromJson(jsonData, "maxDistance", maxDistance);
			Convert::fromJson(jsonData, "aggressionBaseScore", aggressionBaseScore);
			Convert::fromJson(jsonData, "treeStateOffset", treeStateOffset);
		}

		void Debug_Gui() override;
	};

	/// <summary>
	/// 積極性を計算するためのパラメータ
	/// </summary>
	struct AggressionWeights : public AOENGINE::IJsonConverter {
		float base;			// 基本
		float health;		// ヘルス
		float distance;		// 距離

		AggressionWeights() {
			SetGroupName("Boss");
			SetName("AggressionWeights");
		}

		json ToJson(const std::string& id) const override {
			return AOENGINE::JsonBuilder(id)
				.Add("base", base)
				.Add("health", health)
				.Add("distance", distance)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			Convert::fromJson(jsonData, "base", base);
			Convert::fromJson(jsonData, "health", health);
			Convert::fromJson(jsonData, "distance", distance);
		}

		void Debug_Gui() override;
	};

public:

	Boss() = default;
	~Boss() = default;

public:

	// 終了処理
	void Finalize();
	// 初期化
	void Init() override;
	// 更新
	void Update() override;
	// 編集
	void Debug_Gui() override;

public:		// menber method

	/// <summary>
	/// Damageを与える処理
	/// </summary>
	/// <param name="_takeDamage"></param>
	void Damage(float _takeDamage);

	/// <summary>
	/// スタンから解除される処理
	/// </summary>
	void ResetStan();

	/// <summary>
	/// Targetの方向を向く処理
	/// </summary>
	bool TargetLook();

private:

	/// <summary>
	/// 積極度の計算
	/// </summary>
	void CalcAggression();

public:

	// state
	StateMachine<Boss>* GetState() { return stateMachine_.get(); }

	AI::BehaviorTree* GetBehaviorTree() { return behaviorTree_; }

	// targetTransform
	void SetTargetTransform(AOENGINE::WorldTransform* _transform) { pTargetTransform_ = _transform; }
	AOENGINE::WorldTransform* GetTargetTransform() const { return pTargetTransform_; }

	const Math::Vector3& GetTargetPos() const { return targetPos_; }

	// bulletManager
	void SetBulletManager(BossBulletManager* _manager) { pBossBulletManager_ = _manager; }
	BossBulletManager* GetBulletManager() { return pBossBulletManager_; }

	// armor
	Armors* GetPulseArmor() { return pulseArmor_.get(); }

	// UI
	void SetUIs(BossUIs* _pBossUIs) { pBossUIs_ = _pBossUIs; }
	BossUIs* GetUIs() { return pBossUIs_; }

	bool GetIsAlive() const { return isAlive_; }

	// break
	void SetIsBreak(bool _isBreak) { isBreak_ = _isBreak; }
	bool GetIsBreak() const { return isBreak_; }

	// stan
	void SetIsStan(bool isStan) { isStan_ = isStan; }
	bool GetIsStan() const { return isStan_; }

	// targetDead
	void SetIsTargetDead(bool _isTargetDead) { isTargetDead_ = _isTargetDead; }
	bool GetIsTargetDead() const { return isTargetDead_; }

	void SetIsArmorDeploy(bool _isDeployArmor) { isDeployingArmor_ = _isDeployArmor; }
	bool GetIsArmorDeploy() const { return isDeployingArmor_; }

	void SetIsAttack(bool _isAttack) { isAttack_ = _isAttack; }
	bool GetIsAttack() const { return isAttack_; }

	void SetIsMove(bool _isMove) { isMove_ = _isMove; }

	// Treeを止める
	void SetExecute(bool _isStop) { behaviorTree_->SetExecute(_isStop); }

	// phase
	void SetPhase(BossPhase _phase) { phase_ = _phase; }
	BossPhase GetPhase() const { return phase_; }

	const Parameter& GetParameter() const { return param_; }
	const Parameter& GetInitParameter() const { return initParam_; }

	void SetStanRemainingTime(float _time) { stanRemainingTime_ = _time; }
	float GetStanRemainingTime() const { return stanRemainingTime_; }

	float GetAggressionScore() const { return aggressionScore_; }

	void SetFollowCamera(FollowCamera* pFollowCamera) { pFollowCamera_ = pFollowCamera; }
	FollowCamera* GetFollowCamera() const { return pFollowCamera_; }

	float GetAttackArmorDamage() const { return param_.attackArmorDamage; }

private:

	// ポインタ  --------------------------------------------------
	
	BossBulletManager* pBossBulletManager_ = nullptr;

	std::unique_ptr<AttackArmor> attackArmor_ = nullptr;

	FollowCamera* pFollowCamera_ = nullptr;

	// state --------------------------------------------------
	std::unique_ptr<StateMachine<Boss>> stateMachine_;

	BossPhase phase_;

	ActionStrategy actionStrategy_;
	float aggressionScore_;  // 攻勢値

	// weapon ------------------------------------------------
	std::unique_ptr<Armors> pulseArmor_;

	// Parameter --------------------------------------------------
	Parameter param_;
	Parameter initParam_;

	bool isAlive_;
	bool isBreak_;
	bool isStan_;
	bool isTargetDead_;
	bool isDeployingArmor_;
	bool isAttack_;
	bool isMove_;

	float stanRemainingTime_; // スタンの残り時間

	AggressionWeights aggressionWeights_;

	// AI --------------------------------------------------
	AI::BehaviorTree* behaviorTree_;
	std::unique_ptr<AI::Blackboard> blackboard_;

	// Playerの状態 --------------------------------------------------

	AOENGINE::WorldTransform* pTargetTransform_;
	Math::Vector3 targetPos_;

	// UI -----------------------------------------------------------
	BossUIs* pBossUIs_;
};