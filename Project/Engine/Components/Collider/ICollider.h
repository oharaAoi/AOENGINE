#pragma once
#include <functional>
#include <string>
#include <variant>
#include "Engine/Lib/Math/MathStructures.h"

enum CollisionFlags {
	NONE = 0b00,
	ENTER = 0b01,
	EXIT = 0b10,
	STAY = 0b11,
};

enum class ColliderShape {
	SPHERE,
	AABB,
	OBB
};

/// <summary>
/// Colliderの基底クラス
/// </summary>
class ICollider {
public:

	using CollisionFunctions = std::function<void(ICollider* const, ICollider* const)>;

	struct CallBackKinds {
		CollisionFunctions enter;
		CollisionFunctions stay;
		CollisionFunctions exit;
	};

public:

	ICollider() = default;
	virtual ~ICollider() = default;

	virtual void Init(uint32_t bitTag, ColliderShape shape) = 0;
	virtual void Update(const QuaternionSRT& srt) = 0;
	virtual void Draw() const {};

public:

	/// <summary>
	/// 状態の変更
	/// </summary>
	void SwitchCollision(ICollider* partner);

	/// <summary>
	/// 当たっている相手を削除する
	/// </summary>
	/// <param name="partner"></param>
	void DeletePartner(ICollider* partner);

	/// <summary>
	/// 衝突時にコールバック関数を呼び出す
	/// </summary>
	/// <param name="other"></param>
	void OnCollision(ICollider* other);

	/// <summary>
	/// 最初の衝突時に呼ばれる関数の設定
	/// </summary>
	/// <param name="callback"></param>
	void SetCollisionEnter(CollisionFunctions callback) {
		callBacks_.enter = callback;
	}

	/// <summary>
	/// 最初の衝突時に呼ばれる関数の設定
	/// </summary>
	/// <param name="callback"></param>
	void SetCollisionStay(CollisionFunctions callback) {
		callBacks_.stay = callback;
	}

	/// <summary>
	/// 最初の衝突時に呼ばれる関数の設定
	/// </summary>
	/// <param name="callback"></param>
	void SetCollisionExit(CollisionFunctions callback) {
		callBacks_.exit = callback;
	}

	// --------------- 機能しているかの設定・取得 -------------- //
	void SetIsActive(bool isActive) { isActive_ = isActive; }
	bool GetIsActive() const { return isActive_; }

	// --------------- categoryの設定・取得 -------------- //
	void SetCategoryBit(uint32_t bit) { categoryBits_ = bit; }
	uint32_t GetCategoryBit() const { return categoryBits_; }

	// --------------- maskの設定・取得 -------------- //
	void SetMaskBits(uint32_t bit) { maskBits_ |= bit; }
	uint32_t GetMaskBits() const { return maskBits_; }

	// --------------- shapeの設定・取得 -------------- //
	void SetShape(const std::variant<Sphere, AABB, OBB>& shape) { shape_ = shape; }
	const std::variant<Sphere, AABB, OBB>& GetShape() const { return shape_; }

	// --------------- stateの設定・取得 -------------- //
	void SetCollisionState(int stateBit) { collisionState_ = stateBit; }
	int GetCollisionState() const { return collisionState_; }

	// ------------ 半径の設定・取得 ------------ // 
	void SetRadius(const float& radius) { std::get<Sphere>(shape_).radius = radius; }
	float GetRadius() const { return std::get<Sphere>(shape_).radius; }

	// ------------ Colliderの中心座標 ------------ // 
	const Vector3& GetCenterPos() const { return centerPos_; }

	// ------------ size ------------ // 
	void SetSize(const Vector3& size) { size_ = size; }

private:

	/// <summary>
	/// 最初の衝突時に呼ばれる関数
	/// </summary>
	/// <param name="other">: 他の衝突物</param>
	void OnCollisionEnter(ICollider* other);

	/// <summary>
	/// 衝突中に呼ばれる関数
	/// </summary>
	/// <param name="other">: 他の衝突物</param>
	void OnCollisionStay(ICollider* other);

	/// <summary>
	/// 衝突しなくなったら呼ばれる関数
	/// </summary>
	/// <param name="other">: 他の衝突物</param>
	void OnCollisionExit(ICollider* other);

protected:

	bool isActive_ = false;
	
	// カテゴリ
	uint32_t categoryBits_; // 自分が属しているカテゴリ
	uint32_t maskBits_;     // 誰と衝突してもいいかのマスク

	// 形状
	std::variant<Sphere, AABB, OBB> shape_;
	// 当たり判定の状態
	int collisionState_;
	// Colliderの中心座標
	Vector3 centerPos_;
	// AABBやOBBで使用するsize
	Vector3 size_;

	std::unordered_map<ICollider*, int> collisionPartnersMap_;
	
	// 衝突時のcallBack
	CallBackKinds callBacks_;
};

