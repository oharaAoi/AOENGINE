#pragma once
#include "Engine/Lib/Math/MathStructures.h"
#include <variant>

//================================================================================================//
//									当たり判定の関数群												　//
//================================================================================================//

/// <summary>///
/// 球同士の当たり判定
/// </summary>///
/// <param name="s1">球1</param>///
/// <param name="s2">球2</param>///
/// <returns></returns>
bool CheckCollision(const Math::Sphere& s1, const Math::Sphere& s2);

/// <summary>
/// Math::OBB同士の当たり判定
/// </summary>
/// <param name="colliderA">: Math::OBBA</param>
/// <param name="colliderB">: Math::OBBB</param>
/// <returns></returns>
bool CheckCollison(const Math::OBB& colliderA, const Math::OBB& colliderB);

/// <summary>
/// 当たり判定
/// </summary>
/// <param name="aabb1">箱1</param>
/// <param name="aabb2">箱2</param>
/// <returns>true: 当たっている false: 当たっていない</returns>
bool CheckCollision(const Math::AABB& aabb1, const Math::AABB& aabb2);

/// <summary>
/// 球との当たり判定
/// </summary>
/// <param name="aabb">箱</param>
/// <param name="sphere">球</param>
/// <returns>true: 当たっている false: 当たっていない</returns>
bool CheckCollisionAABBandSphere(const Math::AABB& aabb, const Math::Sphere& sphere);

/// <summary>
/// Math::OBBと球の当たり判定
/// </summary>
///  <param name="obb"></param>
/// <param name="sphere"></param>
///  <returns></returns>
bool CheckCollisionOBBandSphere(const Math::OBB& obb, const Math::Sphere& sphere);

/// <summary>
/// Math::OBBとMath::AABBの当たり判定
/// </summary>
/// <param name="obb"></param>
/// <param name="aabb"></param>
/// <returns></returns>
bool CheckCollisionAABBandOBB(const Math::OBB& obb, const Math::AABB& aabb);

/// <summary>
/// Math::SphereとMath::Lineの当たり判定
/// </summary>
/// <param name="sphere"></param>
/// <param name="line"></param>
/// <returns></returns>
bool CheckCollisionSphereAndLine(const Math::Sphere& sphere, const Math::Line& line);

/// <summary>///
/// Math::AABBとMath::Lineの当たり判定
/// </summary>///
///  <param name="aabb">箱</param>
/// <param name="segment">線分</param>
/// <returns>true: 当たっている false: 当たっていない</returns>
bool CheckCollisionAABBandLine(const Math::AABB& aabb, const Math::Line& line);

/// <summary>
/// Math::OBBとMath::Lineの当たり判定
/// </summary>
/// <param name="obb"></param>
/// <param name="line"></param>
/// <returns></returns>
bool CheckCollisionOBBandLine(const Math::OBB& obb, const Math::Line& line);


//================================================================================================//
//								当たり判定の呼び出し関数群											　//
//================================================================================================//

bool CheckCollision(const Math::AABB& aabb, const Math::Sphere& sphere);
bool CheckCollision(const Math::Sphere& sphere, const Math::AABB& aabb);

bool CheckCollision(const Math::OBB& obb, const Math::Sphere& sphere);
bool CheckCollision(const Math::Sphere& sphere, const Math::OBB& obb);

bool CheckCollision(const Math::OBB& obb, const Math::AABB& aabb);
bool CheckCollision(const Math::AABB& aabb, const Math::OBB& obb);

bool CheckCollision(const Math::Sphere& sphere, const Math::Line& line);
bool CheckCollision(const Math::AABB& aabb, const Math::Line& line);
bool CheckCollision(const Math::OBB& obb, const Math::Line& line);

bool CheckCollision(const std::variant<Math::Sphere, Math::AABB, Math::OBB, Math::Line>& shape1,
                    const std::variant<Math::Sphere, Math::AABB, Math::OBB, Math::Line>& shape2);