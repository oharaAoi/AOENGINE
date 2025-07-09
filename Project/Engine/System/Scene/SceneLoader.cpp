#include "SceneLoader.h"
#include "Engine/Utilities/Logger.h"
#include <fstream>

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
	if (objectJson.contains("collider")) {
		const json& collider = objectJson["collider"];
		objectData.colliderType = collider["type"];

		objectData.colliderCenter.x = (float)collider["center"][0];
		objectData.colliderCenter.y = (float)collider["center"][2];
		objectData.colliderCenter.z = (float)collider["center"][1];

		if (objectData.colliderType == "BOX") {
			objectData.colliderSize.x = (float)collider["size"][0];
			objectData.colliderSize.y = (float)collider["size"][2];
			objectData.colliderSize.z = (float)collider["size"][1];

		} else if (objectData.colliderType == "SPHERE") {
			objectData.colliderRadius = (float)collider["radius"];
		}
	}

	if (objectJson.contains("collider_tag")) {
		objectData.colliderTag = objectJson["collider_tag"];
	}

	if (objectJson.contains("collisionFilter_tags")) {
		objectData.collisionFilter = objectJson["collisionFilter_tags"].get<std::vector<std::string>>();
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
