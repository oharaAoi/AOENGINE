#include "SceneLoader.h"
#include <fstream>

SceneLoader* SceneLoader::GetInstance() {
	static SceneLoader instance;
	return &instance;
}

void SceneLoader::Init() {
}

void SceneLoader::Load(const std::string& directory, const std::string& fileName, const std::string& extension) {
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

	// "objects"のアイテムを走査
	for (json& object : deserialized["objects"]) {
		assert(object.contains("type"));

		// 種別を取得
		std::string type = object["type"].get<std::string>();

		// "MESH"
		if (type.compare("MESH") == 0) {
			// 要素を追加
			levelData_->objects.emplace_back(SceneLoader::Objects{});
			SceneLoader::Objects& objectData = levelData_->objects.back();

			objectData.meshName = object["name"];

			if (object.contains("file_name")) {
				// ファイル名
				objectData.modelName = object["file_name"];
			}

			// トランスフォームのパラメータを読み込む
			json& transform = object["transform"];
			// 平行移動
			objectData.srt.translate.x = (float)transform["translation"][0];
			objectData.srt.translate.y = (float)transform["translation"][2];
			objectData.srt.translate.z = (float)transform["translation"][1];
			// 回転角
			Vector3 rotate = CVector3::ZERO;
			rotate.x = -(float)transform["rotation"][0];
			rotate.y = -(float)transform["rotation"][2];
			rotate.z = -(float)transform["rotation"][1];
			objectData.srt.rotate = Quaternion::ToQuaternion(Vector4(rotate.x, rotate.y, rotate.z, 0.0f));
			// スケーリング
			objectData.srt.scale.x = (float)transform["scaling"][0];
			objectData.srt.scale.y = (float)transform["scaling"][2];
			objectData.srt.scale.z = (float)transform["scaling"][1];

		}
	}
}

SceneLoader::Objects SceneLoader::GetObjects(const std::string& objName) const {
	for (size_t index = 0; index < levelData_->objects.size(); ++index) {
		if (levelData_->objects[index].meshName == objName) {
			return levelData_->objects[index];
		}
	}
	// 見つかりませんでしたのログを出す
	return Objects();
}
