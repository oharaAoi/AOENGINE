#pragma once
// engine
#include "Engine/Module/Components/Effect/BaseParticles.h"
// Game
#include "Game/Actor/Weapon/BaseWeapon.h"

/// <summary>
/// 火焔放射の武器
/// </summary>
class Flamethrower :
	public BaseWeapon {
public: // データ構造体

	struct FlamethrowerParam : AOENGINE::IJsonConverter {
		Math::Vector3 pos;

		FlamethrowerParam() {
			SetGroupName("Weapon");
			SetName("Flamethrower");
		}

		json ToJson(const std::string& id) const override {
			return AOENGINE::JsonBuilder(id)
				.Add("pos", pos)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			Convert::fromJson(jsonData, "pos", pos);
		}

		void Debug_Gui() override;
	};

public: // コンストラクタ

	Flamethrower() = default;
	~Flamethrower() = default;

public:

	// 終了
	void Finalize() override;
	// 初期化
	void Init() override;
	// 編集
	void Debug_Gui() override;

public:		// member method

	// 攻撃
	bool Attack(const AttackContext& cxt) override;

private:

	FlamethrowerParam flamethrowerParam_;

	AOENGINE::BaseParticles* flameParticle_ = nullptr;

};

