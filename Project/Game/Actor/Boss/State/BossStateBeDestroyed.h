#pragma once
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Engine/Lib/Math/Quaternion.h"
#include "Game/State/ICharacterState.h"

class Boss;

/// <summary>
/// Bossが撃破された状態
/// </summary>
class BossStateBeDestroyed :
	public ICharacterState<Boss> {
public:

	struct Parameter : public IJsonConverter {
		float slowTime = 1.0f;
		float breakTime = 3.0f;

		Parameter() { 
			SetGroupName("BossState");
			SetName("BossStateBeDestroyed"); 
		}

		json ToJson(const std::string& id) const override {
			return JsonBuilder(id)
				.Add("slowTime", slowTime)
				.Add("breakTime", breakTime)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			fromJson(jsonData, "slowTime", slowTime);
			fromJson(jsonData, "breakTime", breakTime);
		}

		void Debug_Gui() override;
	};

public:

	BossStateBeDestroyed() = default;
	~BossStateBeDestroyed() = default;

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

	float timer_;
	Parameter param_;

	Quaternion preRotate_;
	Quaternion targetRotate_;

};

