#pragma once
#include <string>
#include <unordered_map>
#include <cstdint>
#include <stdexcept>

/// <summary>
/// Colliderのカテゴリの分類を管理しているクラス
/// </summary>
class CollisionLayerManager {
public:

	CollisionLayerManager(); // コンストラクタをprivateに（シングルトン）
	CollisionLayerManager(const CollisionLayerManager&) = delete;
	CollisionLayerManager& operator=(const CollisionLayerManager&) = delete;

	static CollisionLayerManager& getInstance();

	// カテゴリ名を登録（未登録時のみ）
	void registerCategory(const std::string& name);

	// カテゴリ名からビットを取得（未登録なら例外）
	uint32_t getCategoryBit(const std::string& name) const;

private:

	std::unordered_map<std::string, uint32_t> nameToBit_;	// 名前でbitを取得するmap
	uint32_t nextBit_;										// 次のbitの変更値
};

