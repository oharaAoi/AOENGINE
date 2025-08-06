#pragma once
// C++
#include <list>
#include <memory>
// Game
#include "Game/Actor/Player/Bullet/PlayerBullet.h"
#include <Module/Components/Collider/ICollider.h>

/// <summary>
/// PlayerのBulletを管理するクラス
/// </summary>
class PlayerBulletManager {
public:

	PlayerBulletManager() = default;
	~PlayerBulletManager();

	void Init();
	void Update(const Vector3& playerTargetPos);

	BaseBullet* SearchCollider(ICollider* collider);

public: // member method

	/// <summary>
	/// Bossが弾を撃った後にリストに格納する
	/// </summary>
	/// <typeparam name="BulletType">: 弾種類</typeparam>
	/// <typeparam name="...Args">: 可変長引数</typeparam>
	/// <param name="...args">: 各バレットの構造体</param>
	template<typename BulletType, typename... Args>
	BulletType* AddBullet(Args&&... args) {
		auto& bullet = bulletList_.emplace_back(std::make_unique<BulletType>());
		auto type = static_cast<BulletType*>(bullet.get());
		type->Init();
		// BulletTypeに応じたReset呼び出し
		type->Reset(std::forward<Args>(args)...);

		return type;
	}

private:

	std::list<std::unique_ptr<BaseBullet>> bulletList_;

};

