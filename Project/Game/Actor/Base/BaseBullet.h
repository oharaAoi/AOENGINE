#pragma once
#include <string>
// Engine
#include "Engine/Module/Components/GameObject/BaseEntity.h"

enum BulletType {
	NORMAL,
	MISSILE,
};

/// <summary>
/// Baseとなる弾クラス
/// </summary>
class BaseBullet :
	public BaseEntity {
public:

	BaseBullet() = default;
	virtual ~BaseBullet() = default;

	void Finalize();
	void Init(const std::string& bulletName);
	virtual void Update();

	void Reset(const Vector3& pos, const Vector3& velocity);

	void Debug_Gui() override;

public:

	void SetIsAlive(bool isAlive) { isAlive_ = isAlive; }
	bool GetIsAlive() const { return isAlive_; }

	float GetTakeDamage() const { return takeDamage_; }
	void SetTakeDamage(float _damage) { takeDamage_ = _damage; }

	void SetTargetPosition(const Vector3& targetPosition) { targetPosition_ = targetPosition; }

	BulletType GetBulletType() const { return type_; }

protected:

	// State ------------------------------
	Vector3 velocity_;
	Vector3 acceleration_;

	float speed_;

	Vector3 targetPosition_;

	bool isAlive_;

	float takeDamage_ = 10.0f;

	BulletType type_;

};

