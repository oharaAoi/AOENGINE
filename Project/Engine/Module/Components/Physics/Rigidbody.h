#pragma once
#include "Engine/Lib/Math/Vector3.h"
#include "Engine/Lib/Math/Quaternion.h"
#include "Engine/Module/Components/Attribute/AttributeGui.h"

/// <summary>
/// 物理計算ようの
/// </summary>
class Rigidbody :
	public AttributeGui {
public:

	Rigidbody() { Init(); }
	~Rigidbody() = default;

public:

	// 初期化
	void Init();
	// 更新
	void Update();
	// 編集処理
	void Debug_Gui() override;

	/// <summary>
	/// 速度方向の回転を返す
	/// </summary>
	/// <param name="_rotate">: もともとの回</param>
	/// <param name="_rotateT">: 回転率</param>
	/// 	/// <param name="_axis">: 軸</param>
	/// <returns></returns>
	Quaternion LookVelocity(const Quaternion& _rotate, float _rotateT, const Vector3& _axis = CVector3::UP);

public:

	void AddVelocityX(float _x) { velocity_.x += _x; }
	void AddVelocityY(float _y) { velocity_.y += _y; }
	void AddVelocityZ(float _z) { velocity_.z += _z; }
	void SetVelocityX(float _x) { velocity_.x = _x; }
	void SetVelocityY(float _y) { velocity_.y = _y; }
	void SetVelocityZ(float _z) { velocity_.z = _z; }

	void AddVelocity(const Vector3& _velocity) { velocity_ += _velocity; }
	void SetVelocity(const Vector3& _velocity) { velocity_ = _velocity; }
	const Vector3& GetVelocity() const { return velocity_; }

	const Vector3& GetMoveForce() const { return moveForce_; }

	void SetGravity(bool _isGravity) { isGravity_ = _isGravity; }
	bool GetGravity() const { return isGravity_; }

	void SetDrag(float _drag) { drag_ = _drag; }
	float GetDrag() const { return drag_; }

	const Vector3& GetPushbackForce() const { return pushbackForce_; }
	void SetPushbackForce(const Vector3& _force);

private:

	Vector3 velocity_;
	// 移動させる力
	Vector3 moveForce_;

	// 重力に関する変数
	bool isGravity_;
	Vector3 gravityAccel_;
	Vector3 gravityVelocity_;

	// 空気抵抗
	float drag_;

	// 押し戻しに関する変数
	Vector3 pushbackForce_;

};

