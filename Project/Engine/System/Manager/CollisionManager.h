#pragma once
#include <list>
#include <utility>
#include <unordered_map>
#include <functional>
#include "Engine/System/Collision/ColliderCollector.h"
#include "Engine/Module/Components/Collider/ICollider.h"

struct CollisionPair {
	uint32_t categoryA;
	uint32_t categoryB;

	CollisionPair(uint32_t A, uint32_t B) : categoryA(A), categoryB(B) {};

	// unordered_mapのキーにするには比較演算子が必要
	bool operator==(const CollisionPair& other) const {
		return categoryA == other.categoryA && categoryB == other.categoryB;
	}
};

/// <summary>
/// std::hash<CollisionPair>の特殊化を行う
/// </summary>
namespace std {
	template <>
	struct hash<CollisionPair> {
		std::size_t operator()(const CollisionPair& p) const noexcept {
			// categoryA と categoryB を組み合わせてハッシュ化
			std::size_t h1 = std::hash<uint32_t>()(p.categoryA);
			std::size_t h2 = std::hash<uint32_t>()(p.categoryB);
			// AとBを異なるハッシュにする
			return h1 ^ (h2 << 1); // XOR + シフトで衝突を減らす手法
		}
	};
}

/// <summary>
/// Collisionの判定を取る
/// </summary>
class CollisionManager {
public:	// using

	using CollisionFunctions = std::function<void(ICollider* const, ICollider* const)>;

	struct CallBackKinds {
		CollisionFunctions enter;
		CollisionFunctions stay;
		CollisionFunctions exit;
	};


public:

	CollisionManager();
	~CollisionManager();

	void Init();
	
	/// <summary>
	/// すべての当たり判定チェック
	/// </summary>
	void CheckAllCollision();

	/// <summary>
	/// コライダー2つの衝突判定と応答
	/// </summary>
	/// <param name="colliderA">コライダーA</param>
	/// <param name="colliderB">コライダーB</param>
	void CheckCollisionPair(ICollider* colliderA, ICollider* colliderB);

	/// <summary>
	/// CollisionのPairを作成する
	/// </summary>
	/// <param name="colliderA">コライダーA</param>
	/// <param name="colliderB">コライダーB</param>
	/// <param name="callBacks">衝突時の関数群</param>
	void MakeCollisionPair(uint32_t bitA, uint32_t bitB, const CallBackKinds& callBacks);

	/// <summary>
	/// name同士の順序を統一する
	/// </summary>
	/// <param name="a"></param>
	/// <param name="b"></param>
	/// <returns></returns>
	/*std::pair<uint32_t, uint32_t> MakeSortedPair(uint32_t bitA, uint32_t bitB) {
		return (bitA < bitB) ? std::make_pair(bitA, bitB) : std::make_pair(bitA, bitB);
	}*/

	/// <summary>
	/// 衝突している時に行う関数
	/// </summary>
	/// <param name="colliderA">コライダーA</param>
	/// <param name="colliderB">コライダーB</param>
	void OnCollision(ICollider* colliderA, ICollider* colliderB);

	/// <summary>
	/// 衝突しなくなった瞬間に行う関数
	/// </summary>
	/// <param name="colliderA">コライダーA</param>
	/// <param name="colliderB">コライダーB</param>
	void ExitCollision(ICollider* colliderA, ICollider* colliderB);

	void CallBackCollision(ICollider* colliderA, ICollider* colliderB, CallBackKinds callBack);

private:

	ColliderCollector* pColliderCollector_;

	std::unordered_map<CollisionPair, CallBackKinds> callBackFunctions_;

};