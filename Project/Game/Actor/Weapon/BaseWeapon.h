#pragma once
// Engine
#include "Engine/Module/Components/GameObject/BaseEntity.h"
#include "Game/Actor/Player/Bullet/PlayerBulletManager.h"

struct AttackContext {
	Vector3 direction;
	Vector3 target;
};

class BaseWeapon :
	public BaseEntity {
public:

	BaseWeapon() = default;
	virtual ~BaseWeapon() = default;

	virtual void Finalize();
	virtual void Init();

	void Debug_Gui() override;

public:		// member method

	virtual void Attack(const AttackContext& cxt) = 0;

public:		// accessor method

	void SetBulletManager(PlayerBulletManager* _bulletManager) { pBulletManager_ = _bulletManager; }

protected:	// 

	PlayerBulletManager* pBulletManager_ = nullptr;

	float speed_ = 100.f;


};

