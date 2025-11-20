#pragma once
#include <string>
// Engine
#include "Engine/Module/Components/GameObject/BaseEntity.h"

enum BulletType {
	Normal,
	Missile,
	Launcher,
};

/// <summary>
/// Baseとなる弾クラス
/// </summary>
class BaseBullet :
	public BaseEntity {
public:

	BaseBullet() = default;
	virtual ~BaseBullet() = default;

public:

	// 終了処理
	void Finalize();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="bulletName">: バレットの名前</param>
	void Init(const std::string& bulletName);

	// 更新
	virtual void Update();
	
	/// <summary>
	/// 座標や速度の設定
	/// </summary>
	/// <param name="pos">: 座標</param>
	/// <param name="velocity">: 速度</param>
	void Reset(const Vector3& pos, const Vector3& velocity);

	// 編集
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

