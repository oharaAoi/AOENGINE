#include "CollisionLayerManager.h"

CollisionLayerManager::CollisionLayerManager() : nextBit_(1) {
	// 必要ならここで初期カテゴリを登録しておく
	registerCategory("Default");
}

CollisionLayerManager& CollisionLayerManager::getInstance() {
	static CollisionLayerManager instance;
	return instance;
}

void CollisionLayerManager::registerCategory(const std::string& name) {
	// すでに登録済みなら無視
	if (nameToBit_.count(name)) return;

	if (nextBit_ == 0) {
		throw std::runtime_error("No more available bits for collision categories (max 32).");
	}

	nameToBit_[name] = nextBit_;

	nextBit_ <<= 1; // 次のビットへ（例：1→2→4→8→...）
}

uint32_t CollisionLayerManager::getCategoryBit(const std::string& name) const {
	auto it = nameToBit_.find(name);
	if (it == nameToBit_.end()) {
		//return nameToBit_;
	}
	return it->second;
}
