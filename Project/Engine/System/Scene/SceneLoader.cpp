#include "SceneLoader.h"
#include "Engine/Utilities/Logger.h"
#include <fstream>
#include <chrono>
#include <thread>
#include <filesystem>

SceneLoader* SceneLoader::GetInstance() {
	static SceneLoader instance;
	return &instance;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void SceneLoader::Init() {
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ Scene読み込み
///////////////////////////////////////////////////////////////////////////////////////////////

void SceneLoader::Load(const std::string& directory, const std::string& fileName, const std::string& extension) {
	directory_ = directory;
	fileName_ = fileName;
	extension_ = extension;

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

		// 無効フラグがtrueなら配置しない
		if (object.contains("disabled")) {
			bool disabled = object["disabled"].get<bool>();
			if (disabled) {
				continue;
			}
		}

		std::string type = object["type"].get<std::string>();
		if (type.compare("MESH") == 0) {
			levelData_->objects.push_back(LoadObject(object));
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ Objectの読み込み処理
///////////////////////////////////////////////////////////////////////////////////////////////

SceneLoader::Objects SceneLoader::LoadObject(const json& objectJson) {
	SceneLoader::Objects objectData;

	objectData.name = objectJson["name"];

	if (objectJson.contains("file_name")) {
		objectData.modelName = objectJson["file_name"];
	}

	// transform
	const json& transform = objectJson["transform"];
	objectData.srt.translate.x = (float)transform["translation"][0];
	objectData.srt.translate.y = (float)transform["translation"][2];
	objectData.srt.translate.z = (float)transform["translation"][1];

	Vector3 rotate = CVector3::ZERO;
	rotate.x = -(float)transform["rotation"][0];
	rotate.y = -(float)transform["rotation"][2];
	rotate.z = -(float)transform["rotation"][1];
	objectData.srt.rotate = Quaternion::EulerToQuaternion(rotate.x * kToRadian, rotate.y * kToRadian, rotate.z * kToRadian);

	objectData.srt.scale.x = (float)transform["scaling"][0];
	objectData.srt.scale.y = (float)transform["scaling"][2];
	objectData.srt.scale.z = (float)transform["scaling"][1];

	// collider
	if (objectJson.contains("colliders")) {
		const json& collidersJson = objectJson["colliders"];

		for (const auto& colliderJson : collidersJson) {
			ColliderData col;
			col.colliderType = colliderJson["type"];
			col.center.x = (float)colliderJson["center"][0];
			col.center.y = (float)colliderJson["center"][2]; // YとZ入れ替え注意
			col.center.z = (float)colliderJson["center"][1];

			if (col.colliderType == "BOX") {
				col.size.x = (float)colliderJson["size"][0];
				col.size.y = (float)colliderJson["size"][2];
				col.size.z = (float)colliderJson["size"][1];
			} else if (col.colliderType == "SPHERE") {
				col.radius = (float)colliderJson["radius"];
			}

			// タグ系
			if (colliderJson.contains("tag")) {
				col.colliderTag = colliderJson["tag"];
			}
			if (colliderJson.contains("collisionFilter_tags")) {
				col.filter = colliderJson["collisionFilter_tags"].get<std::vector<std::string>>();
			}

			objectData.collidersData.push_back(col);
		}
	}

	// rendering_flag
	if (objectJson.contains("rendering_flag")) {
		objectData.isRendering_ = objectJson["rendering_flag"];
	} else {
		objectData.isRendering_ = true;
	}

	// children
	if (objectJson.contains("children")) {
		for (const json& childJson : objectJson["children"]) {
			objectData.children.push_back(LoadObject(childJson));
		}
	}
	return objectData;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ SceneFileを監視する
///////////////////////////////////////////////////////////////////////////////////////////////

bool SceneLoader::MonitorFileChange() {
	const std::string fullPath = directory_ + fileName_ + extension_;
	std::filesystem::file_time_type lastTime = std::filesystem::last_write_time(fullPath);
	auto currentTime = std::filesystem::last_write_time(fullPath);
	if (currentTime != lastTime) {
		lastTime = currentTime;

		// ファイル再読み込み処理
		Load(directory_, fileName_, extension_);
		return true;
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ Objectを取得する
///////////////////////////////////////////////////////////////////////////////////////////////

SceneLoader::Objects SceneLoader::GetObjects(const std::string& objName) const {
	for (size_t index = 0; index < levelData_->objects.size(); ++index) {
		if (levelData_->objects[index].name == objName) {
			return levelData_->objects[index];
		}
	}
	// 見つかりませんでしたのログを出す
	Logger::Log("[" + objName + "]が見つかりませんでした");
	return Objects();
}
