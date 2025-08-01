#pragma once
#include "Game/Actor/Base/BaseAction.h"
#include "Engine/System/Input/Input.h"
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Engine/System/ParticleSystem/Emitter/GpuParticleEmitter.h"

class Player;


class PlayerActionDeployArmor :
	public BaseAction<Player> {
public:

	struct Parameter : public IJsonConverter {
		float chargeTime = 2.0f;
		Vector3 effectOffset = {0, 2, 0};

		Parameter() { SetName("PlayerActionDeployArmor"); }

		json ToJson(const std::string& id) const override {
			return JsonBuilder(id)
				.Add("chargeTime", chargeTime)
				.Add("effectOffset", effectOffset)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			fromJson(jsonData, "chargeTime", chargeTime);
			fromJson(jsonData, "effectOffset", effectOffset);
		}
	};

public:

	PlayerActionDeployArmor() = default;
	~PlayerActionDeployArmor() override = default;

	void Debug_Gui() override;

	void Build() override;
	void OnStart() override;
	void OnUpdate() override;
	void OnEnd() override;

	void CheckNextAction() override;
	bool IsInput() override;

private:

	Input* pInput_;

	GpuParticleEmitter* chargeEmitter_;

	Parameter parameter_;

};

