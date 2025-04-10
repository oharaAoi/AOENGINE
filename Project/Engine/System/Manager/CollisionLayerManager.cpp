#include "CollisionLayerManager.h"

CollisionLayerManager::CollisionLayerManager() : nextBit_(1) {
	// 必要ならここで初期カテゴリを登録しておく
	RegisterCategory("Default");
}

CollisionLayerManager& CollisionLayerManager::GetInstance() {
	static CollisionLayerManager instance;
	return instance;
}

uint32_t CollisionLayerManager::RegisterCategory(const std::string& name) {
	// すでに登録済みなら無視
	if (nameToBit_.count(name)) return nameToBit_[name];

	if (nextBit_ == 0) {
		throw std::runtime_error("No more available bits for collision categories (max 32).");
	}

	nameToBit_[name] = nextBit_;

	nextBit_ <<= 1; // 次のビットへ（例：1→2→4→8→...）

	return nameToBit_[name];
}

uint32_t CollisionLayerManager::GetCategoryBit(const std::string& name) const {
	auto it = nameToBit_.find(name);
	if (it == nameToBit_.end()) {
		return nameToBit_.at("Default");
	}
	return it->second;
}
