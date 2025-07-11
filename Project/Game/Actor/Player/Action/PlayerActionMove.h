#pragma once
#include "Game/Actor/Base/BaseAction.h"
#include <Lib/Json/IJsonConverter.h>

// 前方宣言
class Player;

class PlayerActionMove :
	public BaseAction<Player>{
public:

	struct Parameter : public IJsonConverter {
		float speed;
		float boostSpeed;
		float moveT = 0.5f;
		float rotateT = 0.5f;
		float decayRate = 2.0f;

		Parameter() { SetName("ActionMove"); }

		json ToJson(const std::string& id) const override {
			return JsonBuilder(id)
				.Add("speed", speed)
				.Add("boostSpeed", boostSpeed)
				.Add("moveT", moveT)
				.Add("rotateT", rotateT)
				.Add("decayRate", decayRate)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			fromJson(jsonData, "speed", speed);
			fromJson(jsonData, "boostSpeed", boostSpeed);
			fromJson(jsonData, "moveT", moveT);
			fromJson(jsonData, "rotateT", rotateT);
			fromJson(jsonData, "decayRate", decayRate);
		}
	};

public:

	PlayerActionMove() = default;
	~PlayerActionMove() override {};

	void Build() override;
	void OnStart() override;
	void OnUpdate() override;
	void OnEnd() override;

	void CheckNextAction() override;
	bool IsInput() override;

	void Debug_Gui() override;

private:	// action

	void Move();

	bool IsDirectionReversed(const Vector3& currentVelocity);

private:

	// Parameter -------------------------------

	const float kDeadZone_ = 0.1f;
	Vector2 inputStick_;

	Vector3 accel_;
	Vector3 velocity_;
	Vector3 preVelocity_;
	
	Parameter param_;
	
	bool isTurnAround_;

	bool preBoost_;
};

