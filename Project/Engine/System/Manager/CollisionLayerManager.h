#pragma once
#include <string>
#include <unordered_map>
#include <cstdint>
#include <stdexcept>
#include <vector>
#include <list>

namespace AOENGINE {

/// <summary>
/// Colliderのカテゴリの分類を管理しているクラス
/// </summary>
class CollisionLayerManager {
public: // constructor

	CollisionLayerManager();
	CollisionLayerManager(const CollisionLayerManager&) = delete;
	CollisionLayerManager& operator=(const CollisionLayerManager&) = delete;

	static CollisionLayerManager& GetInstance();

public: // public method

	void Clear();

	// カテゴリ名を登録（未登録時のみ）
	uint32_t RegisterCategory(const std::string& name);

	// カテゴリ名からビットを取得（未登録なら例外）
	uint32_t GetCategoryBit(const std::string& name);

	// リストごとカテゴリを登録する
	void RegisterCategoryList(const std::list<std::string>& list);

public: // accessor method

	void SetCategoies(const std::vector<std::string>& categoties) { categoties_ = categoties; }
	const std::vector<std::string>& GetCategories() const { return categoties_; }

private: // private variable

	std::unordered_map<std::string, uint32_t> nameToBit_;	// 名前でbitを取得するmap
	uint32_t nextBit_;										// 次のbitの変更値

	std::vector<std::string> categoties_;
};

}