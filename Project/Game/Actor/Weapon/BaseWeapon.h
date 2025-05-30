#pragma once
// Engine
#include "Engine/Module/Components/GameObject/BaseGameObject.h"
#include "Game/Actor/Player/Bullet/PlayerBulletManager.h"

class BaseWeapon :
	public BaseGameObject {
public:

	BaseWeapon() = default;
	virtual ~BaseWeapon() = default;

	void Finalize() override;
	void Init() override;
	void Update() override;
	void Draw() const override;

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

