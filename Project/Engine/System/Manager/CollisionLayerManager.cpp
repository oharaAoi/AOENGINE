#include "CollisionLayerManager.h"
#include <algorithm>
#include <utility>

using namespace AOENGINE;

void AOENGINE::CollisionLayerManager::Clear() {
	nameToBit_.clear();
	nextBit_ = 1;
}

AOENGINE::CollisionLayerManager::CollisionLayerManager() : nextBit_(1) {
	// 必要ならここで初期カテゴリを登録しておく
	RegisterCategory("Default");
}

CollisionLayerManager& AOENGINE::CollisionLayerManager::GetInstance() {
	static CollisionLayerManager instance;
	return instance;
}

uint32_t AOENGINE::CollisionLayerManager::RegisterCategory(const std::string& name) {
	// すでに登録済みなら無視
	if (nameToBit_.count(name)) return nameToBit_[name];

	if (nextBit_ == 0) {
		throw std::runtime_error("No more available bits for collision categories (max 32).");
	}

	nameToBit_[name] = nextBit_;

	nextBit_ <<= 1; // 次のビットへ

	return nameToBit_[name];
}

uint32_t AOENGINE::CollisionLayerManager::GetCategoryBit(const std::string& name) {
	auto it = nameToBit_.find(name);
	if (it == nameToBit_.end()) {
		return RegisterCategory(name);
	}
	return it->second;
}

uint32_t AOENGINE::CollisionLayerManager::GetCategoryBits(const std::vector<std::string>& names) {
	uint32_t bits = 0;
	for (const std::string& name : names) {
		if (name.empty()) { continue; }
		bits |= GetCategoryBit(name);
	}
	return bits;
}

std::vector<std::string> AOENGINE::CollisionLayerManager::GetCategoryNames(uint32_t bits) const {
	std::vector<std::string> names;
	uint32_t remain = bits;

	// 表示順(categoties_)を優先して名前に変換する
	for (const std::string& name : categoties_) {
		auto it = nameToBit_.find(name);
		if (it == nameToBit_.end()) { continue; }
		if ((remain & it->second) == 0) { continue; }

		names.push_back(name);
		remain &= ~it->second;
	}

	// categoties_に無いが登録済みのカテゴリを拾う(bit順で並べて出力を安定させる)
	if (remain != 0) {
		std::vector<std::pair<uint32_t, std::string>> rest;
		for (const auto& [name, bit] : nameToBit_) {
			if ((remain & bit) != 0) {
				rest.emplace_back(bit, name);
			}
		}
		std::sort(rest.begin(), rest.end());
		for (const auto& entry : rest) {
			names.push_back(entry.second);
		}
	}

	return names;
}

void AOENGINE::CollisionLayerManager::RegisterCategoryList(const std::list<std::string>& list) {
	for (auto& id : list) {
		RegisterCategory(id);
	}
}
