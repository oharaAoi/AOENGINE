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

	nextBit_ <<= 1; // 次のビットへ

	return nameToBit_[name];
}

uint32_t CollisionLayerManager::GetCategoryBit(const std::string& name) {
	auto it = nameToBit_.find(name);
	if (it == nameToBit_.end()) {
		return RegisterCategory(name);
	}
	return it->second;
}

void CollisionLayerManager::RegisterCategoryList(const std::list<std::string>& list) {
	for (auto& id : list) {
		RegisterCategory(id);
	}
}
