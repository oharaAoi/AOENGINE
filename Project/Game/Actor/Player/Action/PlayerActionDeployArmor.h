#pragma once
#include "Game/Actor/Base/BaseAction.h"
#include "Engine/System/Input/Input.h"
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Engine/Module/Components/Animation/VectorTween.h"
#include "Engine/System/ParticleSystem/Emitter/GpuParticleEmitter.h"

class Player;

/// <summary>
/// アーマーを展開する処理
/// </summary>
class PlayerActionDeployArmor :
	public BaseAction<Player> {
public:

	struct Parameter : public IJsonConverter {
		float chargeTime = 2.0f;
		Vector3 effectOffset = {0, 2, 0};

		float cameraApproachTime;
		float cameraLeaveTime;
		float cameraOffsetZ;

		Parameter() { SetName("PlayerActionDeployArmor"); }

		json ToJson(const std::string& id) const override {
			return JsonBuilder(id)
				.Add("chargeTime", chargeTime)
				.Add("effectOffset", effectOffset)
				.Add("cameraApproachTime", cameraApproachTime)
				.Add("cameraLeaveTime", cameraLeaveTime)
				.Add("cameraOffsetZ", cameraOffsetZ)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			fromJson(jsonData, "chargeTime", chargeTime);
			fromJson(jsonData, "effectOffset", effectOffset);
			fromJson(jsonData, "cameraApproachTime", cameraApproachTime);
			fromJson(jsonData, "cameraLeaveTime", cameraLeaveTime);
			fromJson(jsonData, "cameraOffsetZ", cameraOffsetZ);
		}

		void Debug_Gui() override;
	};

public:

	PlayerActionDeployArmor() = default;
	~PlayerActionDeployArmor() override = default;

public:

	// ビルド処理
	void Build() override;
	// 初期化
	void OnStart() override;
	// 更新
	void OnUpdate() override;
	// 終了処理
	void OnEnd() override;
	// 次のアクションの判定
	void CheckNextAction() override;
	// actionの入力判定
	bool IsInput() override;
	// 編集処理
	void Debug_Gui() override;

private:

	Input* pInput_;

	GpuParticleEmitter* chargeEmitter_;

	Parameter parameter_;
	
	float cameraInitOffsetZ_;
	VectorTween<float> cameraOffsetZ_;

	bool isDeploy_;

};

