#pragma once
// C++
#include <list>
#include <memory>
// Game
#include "Game/Actor/Base/BaseBullet.h"
#include "Game/Actor/Boss/Bullet/BossMissile.h"

/// <summary>
/// BossのBulletを管理するクラス
/// </summary>
class BossBulletManager {
public:

	BossBulletManager() = default;
	~BossBulletManager() = default;

	void Init();
	void Update();
	void Draw() const;

public: // member method

	/// <summary>
	/// Bossが弾を撃った後にリストに格納する
	/// </summary>
	/// <typeparam name="BulletType">: 弾種類</typeparam>
	/// <typeparam name="...Args">: 可変長引数</typeparam>
	/// <param name="...args">: 各バレットの構造体</param>
	template<typename BulletType, typename... Args>
	void AddBullet(Args&&... args) {
		auto& bullet = bulletList_.emplace_back(std::make_unique<BulletType>());
		bullet->Init();
		// BulletTypeに応じたReset呼び出し
		static_cast<BulletType*>(bullet.get())->Reset(std::forward<Args>(args)...);
	}

	void SetPlayerPosition(const Vector3& pos) { playerPosition_ = pos; }

private:

	std::list<std::unique_ptr<BaseBullet>> bulletList_;

	Vector3 playerPosition_;

};

