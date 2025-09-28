#pragma once
// Engine
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Engine/Module/Components/GameObject/BaseEntity.h"
#include "Game/Actor/Player/Bullet/PlayerBulletManager.h"

struct AttackContext {
	Vector3 direction;
	Vector3 target;
};

class BaseWeapon :
	public BaseEntity {
public:

	struct AttackParam : public IJsonConverter {
		float bulletSpeed = 100;	// 弾の速さ
		int maxBulletsNum = 400;	// 最大弾数
		int fireBulletsNum = 50;	// マガジンの量
		float fireInterval = 0.3f;	// 発射間隔
		float reloadTime = 2.0f;	// リロード時間

		AttackParam() {
			SetName("AttackParam");
			SetGroupName("Weapon");
		}

		json ToJson(const std::string& id) const override {
			return JsonBuilder(id)
				.Add("bulletSpeed", bulletSpeed)
				.Add("maxBulletsNum", maxBulletsNum)
				.Add("fireBulletsNum", fireBulletsNum)
				.Add("fireInterval", fireInterval)
				.Add("reloadTime", reloadTime)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			fromJson(jsonData, "bulletSpeed", bulletSpeed);
			fromJson(jsonData, "maxBulletsNum", maxBulletsNum);
			fromJson(jsonData, "fireBulletsNum", fireBulletsNum);
			fromJson(jsonData, "fireInterval", fireInterval);
			fromJson(jsonData, "reloadTime", reloadTime);
		}

		void Debug_Gui() override;
	};

public:

	BaseWeapon() = default;
	virtual ~BaseWeapon() = default;

	virtual void Finalize();
	virtual void Init();
	virtual void Update();

	void Debug_Gui() override;

public:		// member method

	void AttackAfter();

	virtual void Attack(const AttackContext& cxt) = 0;

public:		// accessor method

	void SetBulletManager(PlayerBulletManager* _bulletManager) { pBulletManager_ = _bulletManager; }

protected:	// 

	PlayerBulletManager* pBulletManager_ = nullptr;

	AttackParam attackParam_;

	// 攻撃に関するパラメータ
	bool isCanAttack_ = true;
	float coolTime_ = 0.0f;
	int fireCount_ = 0;

};

