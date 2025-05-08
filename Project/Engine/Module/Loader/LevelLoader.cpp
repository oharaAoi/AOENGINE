#include "LevelLoader.h"
#include <fstream>

void LevelLoader::Load(const std::string& directory, const std::string& fileName, const std::string& extension) {
	const std::string fullPath = directory + fileName + extension;

	// ファイルストリーム
	std::ifstream file;

	// ファイルを開く
	file.open(fullPath);
	// 開けたかのチェック
	if (file.fail()) {
		assert("scene faileを開けませんでした");
	}

	// fileの中身をjsonに書き出す
	json deserialized;
	// 解凍
	file >> deserialized;

	// 正しいレベルデータファイルかをチェックする
	assert(deserialized.is_object());
	assert(deserialized.contains("name"));
	assert(deserialized["name"].is_string());

	// "name"を文字列として取得
	std::string name = deserialized["name"].get<std::string>();
	assert(name.compare("scene") == 0);

	// -------------------------------------------------
	// ↓ ファイルからobjectを走査する
	// -------------------------------------------------

	levelData_ = nullptr;
	levelData_ = std::make_unique<LevelData>();



}
