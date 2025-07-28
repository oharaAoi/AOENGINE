#include "JsonItems.h"
#include <iostream>
#include <fstream>

const std::string JsonItems::kDirectoryPath_ = "./Game/Assets/GameData/JsonItems/";
std::string JsonItems::nowSceneName_ = "";

namespace fs = std::filesystem;

JsonItems* JsonItems::GetInstance() {
	static JsonItems instance;
	return &instance;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　初期化処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void JsonItems::Init(const std::string& nowScene) {
	nowSceneName_ = nowScene;

	// ------------------------------------------
	// ↓ ディレクトリがなければ作成する
	// ------------------------------------------
	std::filesystem::path dire(kDirectoryPath_ + nowSceneName_ + "/");
	if (!std::filesystem::exists(kDirectoryPath_ + nowSceneName_ + "/")) {
		std::filesystem::create_directories(kDirectoryPath_ + nowSceneName_ + "/");
	}

	LoadAllFile();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　読み込みを行う
//////////////////////////////////////////////////////////////////////////////////////////////////

void JsonItems::LoadAllFile() {
	jsonMap_.clear();
	std::filesystem::directory_iterator rootDir(kDirectoryPath_ + "/" + nowSceneName_ );
	for (const fs::directory_entry& entryDir : fs::directory_iterator(rootDir)) {
		if (entryDir.is_directory()) {
			// サブディレクトリの名前を取得
			const fs::path& subDirPath = entryDir.path();

			for (const fs::directory_entry& subEntry : fs::directory_iterator(subDirPath)) {
				// サブディレクトリ内のファイルパスを取得
				const fs::path& filePath = subEntry.path();
				// ファイル拡張子を取得
				std::string extension = filePath.extension().string();

				// .jsonファイル以外はスキップ
				if (extension.compare(".json") != 0) {
					continue;
				}

				Load(subDirPath.stem().string(), filePath.stem().string());
			}
		}
	}

	// effect読み込み
	std::filesystem::directory_iterator rootEffectDir(kDirectoryPath_ + "/" + "Effect");
	for (const fs::directory_entry& entryDir : fs::directory_iterator(rootEffectDir)) {
		if (entryDir.is_directory()) {
			// サブディレクトリの名前を取得
			const fs::path& subDirPath = entryDir.path();

			for (const fs::directory_entry& subEntry : fs::directory_iterator(subDirPath)) {
				// サブディレクトリ内のファイルパスを取得
				const fs::path& filePath = subEntry.path();
				// ファイル拡張子を取得
				std::string extension = filePath.extension().string();

				// .jsonファイル以外はスキップ
				if (extension.compare(".json") != 0) {
					continue;
				}

				Load(subDirPath.stem().string(), filePath.stem().string(), "Effect");
			}
		}
	}
}

void JsonItems::SaveAllFile() {
	for (const auto& [groupId, converterGroup] : jsonConverterMap_) {
		json groupResult;
		for (const auto& [funcName, func] : converterGroup.items) {
			Save(groupId, func(converterGroup.key));
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　Groupの追加
//////////////////////////////////////////////////////////////////////////////////////////////////

void JsonItems::AddGroup(const std::string& groupName, const json& jsonData) {
	std::string rootKey;
	if (jsonData.is_object() && !jsonData.empty()) {
		rootKey = jsonData.begin().key();
	}
	
	jsonMap_[groupName].items.emplace(rootKey, jsonData);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　保存を行う
//////////////////////////////////////////////////////////////////////////////////////////////////

void JsonItems::Save(const std::string& groupName, const json& saveData, const std::string& rootFold) {
	// 最上位キーの取得
	if (saveData.is_object() && !saveData.empty()) {
		// 最上位のキーの名前をファイル名とする
		std::string rootKey = saveData.begin().key();
		std::string filePath;
		std::filesystem::path dirPath;
		// ファイルパスの作成
		if (rootFold == "") {
			filePath = kDirectoryPath_ + nowSceneName_ + "/" + groupName + "/" + rootKey + ".json";
			dirPath = std::filesystem::path(kDirectoryPath_ + nowSceneName_ + "/" + groupName);
		} else {
			filePath = kDirectoryPath_ + rootFold + "/" + groupName + "/" + rootKey + ".json";
			dirPath = std::filesystem::path(kDirectoryPath_ + rootFold + "/" + groupName);
		}
		

		// -------------------------------------------------
		// ↓ ディレクトリがなければ作成を行う
		// -------------------------------------------------
		if (!std::filesystem::exists(dirPath)) {
			std::filesystem::create_directories(dirPath);
			std::cout << "Created directory: " << dirPath << std::endl;
		}

		// -------------------------------------------------
		// ↓ ファイルを開けるかのチェックを行う
		// -------------------------------------------------
		std::ofstream outFile(filePath);
		if (outFile.fail()) {
			std::string message = "Faild open data file for write\n";
			//Log(message);
			assert(0);
			return;
		}

		// -------------------------------------------------
		// ↓ ファイルに実際に書き込む
		// -------------------------------------------------
		outFile << std::setw(4) << saveData << std::endl;
		outFile.close();

		//Log("JSON data saved as: " + filePath + "\n");
	} else {
		//Log("Invalid or empty JSON data\n");
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　読み込みを行う
//////////////////////////////////////////////////////////////////////////////////////////////////

void JsonItems::Load(const std::string& groupName, const std::string& rootKey, const std::string& rootFold) {
	std::string filePath;
	// ファイルパスの作成
	if (rootFold == "") {
		filePath = kDirectoryPath_ + nowSceneName_ + "/" + groupName + "/" + rootKey + ".json";
	} else {
		filePath = kDirectoryPath_ + rootFold + "/" + groupName + "/" + rootKey + ".json";
	}

	// 読み込み用ファイルストリーム
	std::ifstream ifs;
	// ファイルを読み込みように開く
	ifs.open(filePath);

	if (ifs.fail()) {
		std::string message = "not Exist " + filePath + ".json";
		assert(0);
		return;
	}

	json root;
	// json文字列からjsonのデータ構造に展開
	ifs >> root;
	// ファイルを閉じる
	ifs.close();

	// 値の追加
	GetInstance()->AddGroup(groupName, root);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　値を取得する
//////////////////////////////////////////////////////////////////////////////////////////////////

json JsonItems::GetData(const std::string& groupName, const std::string& rootKey) {
	return GetInstance()->GetValue(groupName, rootKey);
}

json JsonItems::GetValue(const std::string& groupName, const std::string& rootKey) {
	if (jsonMap_.contains(groupName) && jsonMap_[groupName].items.contains(rootKey)) {
		return jsonMap_[groupName].items[rootKey];
	}
	return nullptr;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　各構造体で宣言した保存関数をmapに格納しておく
//////////////////////////////////////////////////////////////////////////////////////////////////

void JsonItems::AddConverter(const std::string& groupName, const std::string& rootKey, std::function<json(const std::string&)> function) {
	GetInstance()->AddConverterGroup(groupName, rootKey, function);
}

void JsonItems::AddConverterGroup(const std::string& groupName, const std::string& rootKey, std::function<json(const std::string&)> function) {
	jsonConverterMap_[groupName].items[rootKey] = function;
	jsonConverterMap_[groupName].key = rootKey;
}

