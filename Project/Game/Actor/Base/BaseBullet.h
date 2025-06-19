#pragma once
#include <string>
// Engine
#include "Engine/Module/Components/GameObject/BaseEntity.h"

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

#ifdef _DEBUG
	void Debug_Gui() override;
#endif // _DEBUG

public:

	void SetIsAlive(bool isAlive) { isAlive_ = isAlive; }
	bool GetIsAlive() const { return isAlive_; }

	void SetTargetPosition(const Vector3& targetPosition) { targetPosition_ = targetPosition; }

protected:

	// State ------------------------------
	Vector3 velocity_;
	Vector3 acceleration_;

	float speed_;

	Vector3 targetPosition_;

	bool isAlive_;

};

