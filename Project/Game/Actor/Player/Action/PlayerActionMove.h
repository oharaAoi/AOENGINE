#pragma once
#include <memory>
#include "Game/Actor/Base/BaseAction.h"
#include <Lib/Json/IJsonConverter.h>
// Effect
#include "Game/Effects/JetParticles.h"
#include "Game/Effects/JetBornParticles.h"
#include "Game/Effects/JetEnergyParticles.h"

// 前方宣言
class Player;

class PlayerActionMove :
	public BaseAction<Player>{
public:

	struct Parameter : public IJsonConverter {
		float speed;

		json ToJson(const std::string& id) const override {
			return JsonBuilder(id)
				.Add("speed", speed)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			fromJson(jsonData, "speed", speed);
		}
	};

public:

	PlayerActionMove() = default;
	~PlayerActionMove();

	void Build() override;
	void OnStart() override;
	void OnUpdate() override;
	void OnEnd() override;

	void CheckNextAction() override;
	bool IsInput() override;

#ifdef _DEBUG
	void Debug_Gui() override;
#endif // _DEBUG


private:	// action

	void Move();

private:

	// NextAction -------------------------------
	std::shared_ptr<BaseAction<Player>> jumpAction_;
	std::shared_ptr<BaseAction<Player>> shotAction_;
	std::shared_ptr<BaseAction<Player>> quickBoostAction_;

	const float kDeadZone_ = 0.1f;
	Vector2 stick_;

	Parameter parameter_;

	// effects -------------------------------------------------
	std::unique_ptr<JetParticles> jetParticles_;
	std::unique_ptr<JetBornParticles> jetBornParticles_;
	std::unique_ptr<JetEnergyParticles> jetEnergyParticles_;

};

