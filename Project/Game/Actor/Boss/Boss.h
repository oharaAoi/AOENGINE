#pragma once
#include <memory>
// Engine
#include "Engine/Module/Components/GameObject/BaseEntity.h"
#include "Engine/Lib/Json/IJsonConverter.h"
// Game
#include "Game/State/StateMachine.h"
#include "Game/Manager/ActionManager.h"
#include "Game/Actor/Boss/Bullet/BossBulletManager.h"
#include "Game/Actor/Weapon/PulseArmor.h"

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

#ifdef _DEBUG
	void Debug_Gui() override;
#endif // _DEBUG

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
	PulseArmor* GetPulseArmor() { return pulseArmor_.get(); }

	const Parameter& GetParameter() const { return param_; }
	const Parameter& GetInitParameter() const { return initParam_; }

private:

	// ポインタ  --------------------------------------------------
	
	BossBulletManager* pBossBulletManager_ = nullptr;

	// state --------------------------------------------------
	std::unique_ptr<StateMachine<Boss>> stateMachine_;

	std::unique_ptr<ActionManager<Boss>> actionManager_;

	// weapon ------------------------------------------------
	std::unique_ptr<PulseArmor> pulseArmor_;

	// Parameter --------------------------------------------------
	Parameter param_;
	Parameter initParam_;

	// Playerの状態 --------------------------------------------------

	Vector3 playerPosition_;

};

