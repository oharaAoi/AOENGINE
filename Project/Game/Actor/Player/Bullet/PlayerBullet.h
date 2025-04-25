#pragma once
#include "Game/Actor/Base/BaseBullet.h"

/// <summary>
/// Playerの基本の弾
/// </summary>
class PlayerBullet :
	public BaseBullet {
public:

	PlayerBullet() = default;
	~PlayerBullet() override;

	void Init();
	void Update();
	void Draw() const;

private:



};

