#pragma once
#include <memory>
// Engine
#include "Engine/Module/Components/GameObject/BaseGameObject.h"
#include "Engine/Module/Components/Attribute/AttributeGui.h"
#include "Engine/Lib/Json/IJsonConverter.h"
// Game
#include "Game/State/StateMachine.h"
#include "Game/Manager/ActionManager.h"
#include "Game/Actor/Boss/Bullet/BossBulletManager.h"

class Boss :
	public AttributeGui {
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

	StateMachine<Boss>* GetState() { return stateMachine_.get(); }

	void SetPlayerPosition(const Vector3& _position) { playerPosition_ = _position; }
	const Vector3& GetPlayerPosition() const { return playerPosition_; }

	void SetBulletManager(BossBulletManager* _manager) { pBossBulletManager_ = _manager; }
	BossBulletManager* GetBulletManager() { return pBossBulletManager_; }

	Vector3 GetPosition() { return boss_->GetPosition(); }

	BaseGameObject* GetGameObject() { return boss_; }
	WorldTransform* GetTransform() { return transform_; }

	const Parameter& GetParameter() const { return param_; }
	const Parameter& GetInitParameter() const { return initParam_; }

private:

	// ポインタ  --------------------------------------------------
	BaseGameObject* boss_;
	WorldTransform* transform_;

	BossBulletManager* pBossBulletManager_ = nullptr;

	// state --------------------------------------------------
	std::unique_ptr<StateMachine<Boss>> stateMachine_;

	std::unique_ptr<ActionManager<Boss>> actionManager_;

	// Parameter --------------------------------------------------
	Parameter param_;
	Parameter initParam_;

	// Playerの状態 --------------------------------------------------

	Vector3 playerPosition_;

};

