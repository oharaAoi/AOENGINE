#include "CollisionManager.h"
#include "Engine/Module/Components/Collider/CollisionFunctions.h"
#include "Engine/System/Manager/CollisionLayerManager.h"
#include "Engine/System/Collision/PenetrationResolution.h"
#include "Engine/Utilities/BitChecker.h"
#include <cmath>
#include <cstdint>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace AOENGINE;

namespace {

constexpr float kBroadPhaseCellSize = 4.0f;

struct CellKey {
	int x;
	int y;
	int z;

	bool operator==(const CellKey& other) const {
		return x == other.x && y == other.y && z == other.z;
	}
};

struct CellKeyHash {
	size_t operator()(const CellKey& key) const {
		const size_t hx = std::hash<int>{}(key.x);
		const size_t hy = std::hash<int>{}(key.y);
		const size_t hz = std::hash<int>{}(key.z);
		return hx ^ (hy << 1) ^ (hz << 2);
	}
};

bool CanCheckCollision(const BaseCollider& colliderA, const BaseCollider& colliderB) {
	if (colliderA.GetCategoryName() != "Default" && colliderB.GetCategoryName() != "Default") {
		return HasBit(colliderA.GetCollisionMaskBit(), colliderB.GetLayerBit());
	}
	return true;
}

Math::AABB GetBroadPhaseAABB(const BaseCollider& collider) {
	return std::visit([](const auto& shape) {
		using Shape = std::decay_t<decltype(shape)>;
		if constexpr (std::is_same_v<Shape, Math::Sphere>) {
			const Math::Vector3 radius(shape.radius, shape.radius, shape.radius);
			return Math::AABB{ .min = shape.center - radius, .max = shape.center + radius, .center = shape.center };
		} else if constexpr (std::is_same_v<Shape, Math::AABB>) {
			return shape;
		} else if constexpr (std::is_same_v<Shape, Math::OBB>) {
			const Math::Vector3 extent =
				Math::Vector3(std::fabs(shape.orientations[0].x) * shape.size.x,
					std::fabs(shape.orientations[0].y) * shape.size.x,
					std::fabs(shape.orientations[0].z) * shape.size.x) +
				Math::Vector3(std::fabs(shape.orientations[1].x) * shape.size.y,
					std::fabs(shape.orientations[1].y) * shape.size.y,
					std::fabs(shape.orientations[1].z) * shape.size.y) +
				Math::Vector3(std::fabs(shape.orientations[2].x) * shape.size.z,
					std::fabs(shape.orientations[2].y) * shape.size.z,
					std::fabs(shape.orientations[2].z) * shape.size.z);
			return Math::AABB{ .min = shape.center - extent, .max = shape.center + extent, .center = shape.center };
		} else {
			const Math::Vector3 end = shape.origin + shape.diff;
			return Math::AABB{ .min = Math::Vector3::Min(shape.origin, end),
				.max = Math::Vector3::Max(shape.origin, end),
				.center = (shape.origin + end) * 0.5f };
		}
	}, collider.GetShape());
}

std::vector<std::pair<BaseCollider*, BaseCollider*>> BuildCandidatePairs(
	const std::vector<BaseCollider*>& colliders, bool includeExistingPartners) {
	std::unordered_map<CellKey, std::vector<size_t>, CellKeyHash> cells;
	std::vector<Math::AABB> bounds(colliders.size());
	std::unordered_set<uint64_t> uniquePairs;
	std::vector<std::pair<BaseCollider*, BaseCollider*>> pairs;
	cells.reserve(colliders.size());
	uniquePairs.reserve(colliders.size() * 4);
	pairs.reserve(colliders.size() * 4);

	for (size_t index = 0; index < colliders.size(); ++index) {
		BaseCollider* collider = colliders[index];
		if (!collider || !collider->GetIsActive()) { continue; }
		bounds[index] = GetBroadPhaseAABB(*collider);
		const CellKey minCell{
			static_cast<int>(std::floor(bounds[index].min.x / kBroadPhaseCellSize)),
			static_cast<int>(std::floor(bounds[index].min.y / kBroadPhaseCellSize)),
			static_cast<int>(std::floor(bounds[index].min.z / kBroadPhaseCellSize)) };
		const CellKey maxCell{
			static_cast<int>(std::floor(bounds[index].max.x / kBroadPhaseCellSize)),
			static_cast<int>(std::floor(bounds[index].max.y / kBroadPhaseCellSize)),
			static_cast<int>(std::floor(bounds[index].max.z / kBroadPhaseCellSize)) };
		for (int x = minCell.x; x <= maxCell.x; ++x) {
			for (int y = minCell.y; y <= maxCell.y; ++y) {
				for (int z = minCell.z; z <= maxCell.z; ++z) {
					cells[{ x, y, z }].push_back(index);
				}
			}
		}
	}

	for (const auto& [cell, indices] : cells) {
		for (size_t a = 0; a < indices.size(); ++a) {
			for (size_t b = a + 1; b < indices.size(); ++b) {
				size_t first = indices[a];
				size_t second = indices[b];
				if (first > second) { std::swap(first, second); }
				if (!CanCheckCollision(*colliders[first], *colliders[second])) { continue; }
				const uint64_t key = (static_cast<uint64_t>(first) << 32) | static_cast<uint64_t>(second);
				if (uniquePairs.insert(key).second) {
					pairs.emplace_back(colliders[first], colliders[second]);
				}
			}
		}
	}

	if (!includeExistingPartners) {
		return pairs;
	}

	// Broad Phaseから外れたフレームでも、前フレームから継続しているペアは
	// CheckCollisionPairへ渡してExitCollisionを確実に発生させる。
	std::unordered_map<BaseCollider*, size_t> colliderIndices;
	colliderIndices.reserve(colliders.size());
	for (size_t index = 0; index < colliders.size(); ++index) {
		if (colliders[index] && colliders[index]->GetIsActive()) {
			colliderIndices.emplace(colliders[index], index);
		}
	}
	for (BaseCollider* collider : colliders) {
		if (!collider || !collider->GetIsActive()) { continue; }
		const auto colliderIt = colliderIndices.find(collider);
		if (colliderIt == colliderIndices.end()) { continue; }
		for (const auto& [partner, state] : collider->GetCollisionPartners()) {
			(void)state;
			const auto partnerIt = colliderIndices.find(partner);
			if (partnerIt == colliderIndices.end()) { continue; }
			size_t first = colliderIt->second;
			size_t second = partnerIt->second;
			if (first > second) { std::swap(first, second); }
			if (!CanCheckCollision(*colliders[first], *colliders[second])) { continue; }
			const uint64_t key = (static_cast<uint64_t>(first) << 32) | static_cast<uint64_t>(second);
			if (uniquePairs.insert(key).second) {
				pairs.emplace_back(colliders[first], colliders[second]);
			}
		}
	}
	return pairs;
}

}

CollisionManager::CollisionManager() {}
CollisionManager::~CollisionManager() {
	Finalize();
}

void CollisionManager::Finalize() {
	pColliderCollector_->Reset();
	auto& layers = AOENGINE::CollisionLayerManager::GetInstance();
	layers.Clear();
}

/////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　初期化処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void CollisionManager::Init() {
	pColliderCollector_ = ColliderCollector::GetInstance();
	pColliderCollector_->Init();
}

/////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　すべての当たり判定チェック
//////////////////////////////////////////////////////////////////////////////////////////////////

void CollisionManager::CheckAllCollision() {
	
	const std::vector<BaseCollider*>& colliderList = pColliderCollector_->GetColliderList();
	for (const auto& [colliderA, colliderB] : BuildCandidatePairs(colliderList, true)) {

			CheckCollisionPair(colliderA, colliderB);
	}
}

void CollisionManager::CheckHorizontalCollision() {
	const std::vector<BaseCollider*>& colliderList = pColliderCollector_->GetColliderList();
	std::unordered_map<BaseCollider*, Math::Vector3> horizontalCorrections;
	auto keepLargestAxisCorrection = [&horizontalCorrections](BaseCollider* collider, const Math::Vector3& correction) {
		Math::Vector3& accumulated = horizontalCorrections[collider];
		if (std::abs(correction.x) > std::abs(accumulated.x)) { accumulated.x = correction.x; }
		if (std::abs(correction.z) > std::abs(accumulated.z)) { accumulated.z = correction.z; }
	};
	for (const auto& [colliderA, colliderB] : BuildCandidatePairs(colliderList, false)) {
			if (!CheckCollision(colliderA->GetShape(), colliderB->GetShape()) ||
				colliderA->GetIsTrigger() || colliderB->GetIsTrigger()) { continue; }
			if (!colliderA->GetIsStatic()) {
				keepLargestAxisCorrection(colliderA, PenetrationResolutionHorizontal(colliderA->GetShape(), colliderB->GetShape()));
			}
			if (!colliderB->GetIsStatic()) {
				keepLargestAxisCorrection(colliderB, PenetrationResolutionHorizontal(colliderB->GetShape(), colliderA->GetShape()));
			}
	}
	// 縦に連続した壁へ同時に当たっても、同じ横補正を壁の個数分加算しない。
	for (const auto& [collider, correction] : horizontalCorrections) {
		collider->SetPushBackDirection(correction);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　コライダー2つの衝突判定と応答
/////////////////////////////////////////////////////////////////////////////////////////////////

void CollisionManager::CheckCollisionPair(BaseCollider* colliderA, BaseCollider* colliderB) {
	if (CheckCollision(colliderA->GetShape(), colliderB->GetShape())) {
		// Colliderの状態を変化させる
		colliderA->SwitchCollision(colliderB);
		colliderB->SwitchCollision(colliderA);

		OnCollision(colliderA, colliderB);

		// 汎用の当たり判定後処理
		colliderA->OnCollision(colliderB);
		colliderB->OnCollision(colliderA);

		if (colliderA->GetIsTrigger() || colliderB->GetIsTrigger()) {
			return;
		}

		if (!colliderA->GetIsStatic()) {
			colliderA->SetPushBackDirection(PenetrationResolution(colliderA->GetShape(), colliderB->GetShape()));
		}

		if (!colliderB->GetIsStatic()) {
			// 押し戻しは「第1引数側を逃がす向き」で返るため、B用は形状の順番を入れ替える
			colliderB->SetPushBackDirection(PenetrationResolution(colliderB->GetShape(), colliderA->GetShape()));
		}
		
	} else {
		ExitCollision(colliderA, colliderB);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　ペアを作成する
/////////////////////////////////////////////////////////////////////////////////////////////////

void CollisionManager::MakeCollisionPair(uint32_t bitA, uint32_t bitB, const CallBackKinds& callBacks) {
	callBackFunctions_[CollisionPair(bitA, bitB)] = callBacks;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　衝突している時に行う関数
/////////////////////////////////////////////////////////////////////////////////////////////////

void CollisionManager::OnCollision(BaseCollider* colliderA, BaseCollider* colliderB) {
	// ペアを作成する
	auto pair = CollisionPair(colliderA->GetLayerBit(), colliderB->GetLayerBit());
	auto reversePair = CollisionPair(colliderB->GetLayerBit(), colliderA->GetLayerBit());

	// ペアがマップに存在するかを確認
	bool isReverse = false;
	auto it = callBackFunctions_.find(pair);
	auto reverseIt = callBackFunctions_.find(reversePair);

	if (it == callBackFunctions_.end()) {
		isReverse = true;
		if (reverseIt == callBackFunctions_.end()) {
			return;
		}
	}
	
	std::pair<BaseCollider*, BaseCollider*> collisionPair;
	CallBackKinds callbacks;
	if (isReverse) {
		callbacks = reverseIt->second;
		collisionPair.first = colliderB;
		collisionPair.second = colliderA;
	} else {
		callbacks = it->second;
		collisionPair.first = colliderA;
		collisionPair.second = colliderB;
	}

	switch (collisionPair.first->GetCollisionState()) {
	case (int)CollisionFlags::Enter:
		if (callbacks.enter) {
			callbacks.enter(collisionPair.first, collisionPair.second);
		}
		break;
	case (int)CollisionFlags::Stay:
		if (callbacks.stay) {
			callbacks.stay(collisionPair.first, collisionPair.second);
		}
		break;
	default:
		break;
	}

	switch (collisionPair.second->GetCollisionState()) {
	case (int)CollisionFlags::Enter:
		if (callbacks.enter) {
			callbacks.enter(collisionPair.first, collisionPair.second);
		}
		break;
	case (int)CollisionFlags::Stay:
		if (callbacks.stay) {
			callbacks.stay(collisionPair.first, collisionPair.second);
		}
		break;
	default:
		break;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　衝突しなくなった瞬間に行う関数
/////////////////////////////////////////////////////////////////////////////////////////////////

void CollisionManager::ExitCollision(BaseCollider* colliderA, BaseCollider* colliderB) {
	// 衝突している状態だったら脱出した状態にする
	for (auto collider : { colliderA, colliderB }) {
		if (collider->GetCollisionState() == (int)CollisionFlags::Stay) {
			collider->SetCollisionState((int)CollisionFlags::Exit);
		} else {
			collider->SetCollisionState((int)CollisionFlags::None);
			collider->DeletePartner(colliderA == collider ? colliderB : colliderA);
		}
	}
}

void CollisionManager::CallBackCollision(BaseCollider* colliderA, BaseCollider* colliderB, CallBackKinds callBack) {
	switch (colliderA->GetCollisionState()) {
	case (int)CollisionFlags::Enter:
		if (callBack.enter) {
			callBack.enter(colliderA, colliderB);
		}
		break;
	case (int)CollisionFlags::Stay:
		if (callBack.stay) {
			callBack.stay(colliderA, colliderB);
		}
		break;
	default:
		break;
	}
}
