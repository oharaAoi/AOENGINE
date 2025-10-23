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

		ShoulderMissileParam() {
			SetGroupName("Weapon");
			SetName("ShoulderMissile");
		}

		json ToJson(const std::string& id) const override {
			return JsonBuilder(id)
				.Add("pos", pos)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			fromJson(jsonData, "pos", pos);
		}

		void Debug_Gui() override;
	};

public:

	ShoulderMissile() = default;
	~ShoulderMissile() override = default;

public:

	// 終了
	void Finalize() override;
	// 初期化
	void Init() override;
	// 更新
	void Update() override;
	// 編集
	void Debug_Gui() override;

public:		// member method

	// 攻撃
	bool Attack(const AttackContext& cxt) override;

	// 弾を打つ処理
	void Shot();

	// 実行
	void Execute(const Vector3& pos);

private:


	ShoulderMissileParam weaponParam_;

	AttackContext attackCxt_;
	bool isReload_;
	bool isFinish_;
	int shotCount_;

};

