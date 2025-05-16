#pragma once
#include <memory>
#include "Game/Actor/Base/BaseAction.h"
#include <Lib/Json/IJsonConverter.h>

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
	~PlayerActionMove() override {};

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
	std::shared_ptr<BaseAction<Player>> shotActionRight_;
	std::shared_ptr<BaseAction<Player>> shotActionLeft_;
	std::shared_ptr<BaseAction<Player>> quickBoostAction_;
	std::shared_ptr<BaseAction<Player>> boostAction_;

	// Parameter -------------------------------
	float actionTimer_;

	const float kDeadZone_ = 0.1f;
	Vector2 stick_;

	Parameter parameter_;
};

