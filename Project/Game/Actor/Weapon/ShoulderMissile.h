#pragma once
#include "Engine/Lib/Json/IJsonConverter.h"
// Game
#include "Game/Actor/Weapon/BaseWeapon.h"

/// <summary>
/// 肩の武器
/// </summary>
class ShoulderMissile :
	public BaseWeapon {
public:

	struct ShoulderMissileParam : IJsonConverter {
		Vector3 pos;

		ShoulderMissileParam() { SetName("ShoulderMissile"); }

		json ToJson(const std::string& id) const override {
			return JsonBuilder(id)
				.Add("pos", pos)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			fromJson(jsonData, "pos", pos);
		}
	};

public:

	ShoulderMissile() = default;
	~ShoulderMissile() override = default;

	void Finalize() override;
	void Init() override;
	
	void Debug_Gui() override;

public:		// member method

	void Attack(const AttackContext& cxt) override;

	void Execute(const Vector3& pos);

private:

	ShoulderMissileParam weaponParam_;

};

