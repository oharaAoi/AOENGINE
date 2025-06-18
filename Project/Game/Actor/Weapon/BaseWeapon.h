#pragma once
// Engine
#include "Engine/Module/Components/GameObject/BaseEntity.h"
#include "Game/Actor/Player/Bullet/PlayerBulletManager.h"

class BaseWeapon :
	public BaseEntity {
public:

	BaseWeapon() = default;
	virtual ~BaseWeapon() = default;

	virtual void Finalize();
	virtual void Init();
	virtual void Update();
	virtual void Draw() const;

#ifdef _DEBUG
	void Debug_Gui() override;
#endif // _DEBUG

public:		// member method

	virtual void Shot(const Vector3& targetPos, uint32_t type) = 0;

public:		// accessor method

	void SetBulletManager(PlayerBulletManager* _bulletManager) { pBulletManager_ = _bulletManager; }

protected:	// 

	PlayerBulletManager* pBulletManager_ = nullptr;

	float speed_ = 100.f;


};

