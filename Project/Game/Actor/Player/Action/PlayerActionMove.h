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

		Parameter() { SetName("ActionMove"); }

		json ToJson(const std::string& id) const override {
			return JsonBuilder(id)
				.Add("speed", speed)
				.Add("boostSpeed", boostSpeed)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			fromJson(jsonData, "speed", speed);
			fromJson(jsonData, "boostSpeed", boostSpeed);
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

	void Deceleration(Vector2 currentInput);

	bool IsDirectionReversed(const Vector3& currentVelocity);

private:

	// Parameter -------------------------------

	const float kDeadZone_ = 0.1f;
	Vector2 inputStick_;

	Vector3 preVelocity_;

	Parameter param_;
	Parameter initParam_;

	int prevNum_ = 6;

	bool isTurnAround_;
};

