#pragma once
#include <memory>
#include "Engine/Module/Components/2d/Sprite.h"
#include "Engine/Module/Components/WorldTransform.h"
#include <Lib/Math/Vector3.h>

/// <summary>
/// レティクル
/// </summary>
class Reticle {
public:

	Reticle() = default;
	~Reticle() = default;

public:

	// 初期化
	void Init();
	// 更新
	void Update();
	// 描画
	void Draw() const;
	// ロックオン
	void LockOn();

	void SetReticlePos(AOENGINE::WorldTransform* targetTransform, const Math::Matrix4x4& vpvpMat);

public:		// accessor method

	Math::Vector3 GetTargetPos() const;

	bool GetLockOn() const { return isLockOn_; }

	void ReleaseLockOn();

	const Math::Vector2& GetPos() const { return reticlePos_; }

private:

	AOENGINE::Sprite* reticle_;

	bool isLockOn_ = false;
	Math::Vector2 defaultPosition_;

	AOENGINE::WorldTransform* targetTransform_;

	Math::Vector2 reticlePos_;


};

