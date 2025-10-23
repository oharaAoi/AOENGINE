#pragma once
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Game/State/ICharacterState.h"
#include "Engine/Lib/Math/MyMath.h"

class Boss;

/// <summary>
/// Bossのスタン状態
/// </summary>
class BossStateStan :
	public ICharacterState<Boss> {
public:	// メンバ構造体

	struct Parameter : public IJsonConverter {
		float stanTime = 5.0f;
		
		Parameter() { SetName("BossStanParameter"); }

		json ToJson(const std::string& id) const override {
			return JsonBuilder(id)
				.Add("stanTime", stanTime)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			fromJson(jsonData, "stanTime", stanTime);
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

};

