#pragma once
// Engine
#include "Engine/Module/Components/GameObject/BaseGameObject.h"

/// <summary>
/// Baseとなる弾クラス
/// </summary>
class BaseBullet :
	public BaseGameObject {
public:

	BaseBullet() = default;
	virtual ~BaseBullet() = default;

	void Finalize() override;
	void Init() override;
	void Update() override;
	void Draw() const override;

	void Reset(const Vector3& pos, const Vector3& velocity);

public:

	void SetIsAlive(bool isAlive) { isAlive_ = isAlive; }
	bool GetIsAlive() const { return isAlive_; }

	void SetTargetPosition(const Vector3& targetPosition) { targetPosition_ = targetPosition; }

protected:

	// State ------------------------------
	Vector3 velocity_;
	Vector3 acceleration_;

	Vector3 targetPosition_;

	bool isAlive_;

};

