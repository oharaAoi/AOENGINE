#pragma once
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Engine/Lib/Math/MyMath.h"
#include "Engine/Module/Components/Effect/BaseParticles.h"
#include "Game/State/ICharacterState.h"

class Boss;

/// <summary>
/// Bossのスタン状態
/// </summary>
class BossStateStan :
	public ICharacterState<Boss> {
public:	// メンバ構造体

	struct Parameter : public AOENGINE::IJsonConverter {
		float stanTime = 5.0f;
		float stanSlowTime = 0.4f;
		float effectRandDistance = 3.0f;
		
		Parameter() { 
			SetGroupName("BossState");
			SetName("BossStanParameter");
		}

		json ToJson(const std::string& id) const override {
			return AOENGINE::JsonBuilder(id)
				.Add("stanTime", stanTime)
				.Add("stanSlowTime", stanSlowTime)
				.Add("effectRandDistance", effectRandDistance)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			fromJson(jsonData, "stanTime", stanTime);
			fromJson(jsonData, "stanSlowTime", stanSlowTime);
			fromJson(jsonData, "effectRandDistance", effectRandDistance);
		}

		void Debug_Gui() override;
	};

public:

	BossStateStan() = default;
	~BossStateStan() = default;

public:

	// 初期化
	void OnStart() override;
	// 更新
	void OnUpdate() override;
	// 終了時
	void OnExit() override;
	// 編集処理
	void Debug_Gui() override;

private:

	Parameter param_;
	float stateTime_;
	float slowTime_;

	BaseParticles* effect_;
};

