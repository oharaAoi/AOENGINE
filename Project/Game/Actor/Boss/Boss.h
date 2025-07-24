#pragma once
#include <memory>
// Engine
#include "Engine/Module/Components/GameObject/BaseEntity.h"
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Engine/Module/Components/AI/BehaviorTree.h"
// Game
#include "Game/State/StateMachine.h"
#include "Game/Manager/ActionManager.h"
#include "Game/Actor/Boss/Bullet/BossBulletManager.h"
#include "Game/Actor/Boss/BossAI.h"
#include "Game/Actor/Weapon/Armors.h"

enum class BossPhase {
	FIRST,
	SECOND,
};

class BossUIs;

class Boss :
	public BaseEntity {
public:

	struct Parameter : public IJsonConverter {
		float health;
		float postureStability;

		Parameter() { SetName("bossParameter"); }

		json ToJson(const std::string& id) const override {
			return JsonBuilder(id)
				.Add("health", health)
				.Add("postureStability", postureStability)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			fromJson(jsonData, "health", health);
			fromJson(jsonData, "postureStability", postureStability);
		}
	};
public:

	Boss() = default;
	~Boss() = default;

	void Finalize();
	void Init();
	void Update();
	void Draw() const;

	void Debug_Gui() override;

public:		// menber method

	void Damage(float _takeDamage);

	void ResetStan();

public:

	// state
	StateMachine<Boss>* GetState() { return stateMachine_.get(); }

	// player座標
	void SetPlayerPosition(const Vector3& _position) { playerPosition_ = _position; }
	const Vector3& GetPlayerPosition() const { return playerPosition_; }

	// bulletManager
	void SetBulletManager(BossBulletManager* _manager) { pBossBulletManager_ = _manager; }
	BossBulletManager* GetBulletManager() { return pBossBulletManager_; }

	// armor
	Armors* GetPulseArmor() { return pulseArmor_.get(); }

	// AI
	BossAI* GetAI() { return AI_.get(); }

	// UI
	void SetUIs(BossUIs* _pBossUIs) { pBossUIs_ = _pBossUIs; }
	BossUIs* GetUIs() { return pBossUIs_; }

	bool GetIsAlive() const { return isAlive_; }

	// break
	void SetIsBreak(bool _isBreak) { isBreak_ = _isBreak; }
	bool GetIsBreak() const { return isBreak_; }

	// stan
	bool GetIsStan() const { return isStan_; }
	void SetIsStan(bool isStan) { isStan_ = isStan; }

	// Treeを止める
	void StopTree(bool _isStop) { behaviorTree_->SetExecute(_isStop); }

	// phase
	void SetPhase(BossPhase _phase) { phase_ = _phase; }
	BossPhase GetPhase() const { return phase_; }

	const Parameter& GetParameter() const { return param_; }
	const Parameter& GetInitParameter() const { return initParam_; }

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

	// AI --------------------------------------------------
	std::unique_ptr<BossAI> AI_;

	std::unique_ptr<BehaviorTree> behaviorTree_;

	// Playerの状態 --------------------------------------------------

	Vector3 playerPosition_;

	// UI -----------------------------------------------------------
	BossUIs* pBossUIs_;

};

