#pragma once
#include <memory>
// Engine
#include "Engine/Module/Components/GameObject/BaseEntity.h"
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Engine/System/AI/BehaviorTree.h"
// Game
#include "Game/State/StateMachine.h"
#include "Game/Actor/Boss/Bullet/BossBulletManager.h"
#include "Game/Actor/Boss/BossEvaluationFormula.h"
#include "Game/Actor/Weapon/Armors.h"

enum class BossPhase {
	First,
	Second,
};

class BossUIs;

/// <summary>
/// Bossクラス
/// </summary>
class Boss :
	public AOENGINE::BaseEntity {
public:

	struct Parameter : public AOENGINE::IJsonConverter {
		float health = 100;
		float postureStability = 100.0f;
		float postureStabilityScrapeRaito = 0.3f;	// 耐久度を削る割合
		float armorCoolTime = 20.0f;
		float angularVelocity = 90.f; // 角速度
		float angularThreshold = 10.f; // 角速度

		std::string worldStatePath = "";		// worldStateのパス

		Parameter() { SetName("bossParameter"); }

		json ToJson(const std::string& id) const override {
			return AOENGINE::JsonBuilder(id)
				.Add("health", health)
				.Add("postureStability", postureStability)
				.Add("postureStabilityScrapeRaito", postureStabilityScrapeRaito)
				.Add("armorCoolTime", armorCoolTime)
				.Add("angularVelocity", angularVelocity)
				.Add("angularThreshold", angularThreshold)
				.Add("worldStatePath", worldStatePath)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			Convert::fromJson(jsonData, "health", health);
			Convert::fromJson(jsonData, "postureStability", postureStability);
			Convert::fromJson(jsonData, "postureStabilityScrapeRaito", postureStabilityScrapeRaito);
			Convert::fromJson(jsonData, "armorCoolTime", armorCoolTime);
			Convert::fromJson(jsonData, "angularVelocity", angularVelocity);
			Convert::fromJson(jsonData, "angularThreshold", angularThreshold);
			Convert::fromJson(jsonData, "worldStatePath", worldStatePath);
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
	void Init();
	// 更新
	void Update();
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

public:

	// state
	StateMachine<Boss>* GetState() { return stateMachine_.get(); }

	AI::BehaviorTree* GetBehaviorTree() { return behaviorTree_; }

	// 評価値計算クラス
	BossEvaluationFormula* GetEvaluationFormula() { return evaluationFormula_.get(); }

	// targetTransform
	void SetTargerTransform(AOENGINE::WorldTransform* _transform) { targetTransform_ = _transform; }
	AOENGINE::WorldTransform* GetTargetTransform() const { return targetTransform_; }

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

	void SetIsArmorDeploy(bool _isDeployArmor) { isArmorDeploy_ = _isDeployArmor; }
	bool GetIsArmorDeploy() const { return isArmorDeploy_; }

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

private:

	// ポインタ  --------------------------------------------------
	
	BossBulletManager* pBossBulletManager_ = nullptr;

	// state --------------------------------------------------
	std::unique_ptr<StateMachine<Boss>> stateMachine_;

	BossPhase phase_;

	// weapon ------------------------------------------------
	std::unique_ptr<Armors> pulseArmor_;

	// Parameter --------------------------------------------------
	Parameter param_;
	Parameter initParam_;

	bool isAlive_;
	bool isBreak_;
	bool isStan_;
	bool isTargetDead_;
	bool isArmorDeploy_;
	bool isAttack_;
	bool isMove_;

	float stanRemainingTime_; // スタンの残り時間

	// AI --------------------------------------------------
	AI::BehaviorTree* behaviorTree_;
	std::unique_ptr<AI::Blackboard> blackboard_;

	std::unique_ptr<BossEvaluationFormula> evaluationFormula_;

	// Playerの状態 --------------------------------------------------

	AOENGINE::WorldTransform* targetTransform_;
	Math::Vector3 targetPos_;

	// UI -----------------------------------------------------------
	BossUIs* pBossUIs_;
};