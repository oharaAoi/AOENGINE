#include "SceneManagerPropertySerializer.h"
#include <fstream>
#include <nlohmann/json.hpp>

void AOENGINE::SceneManagerPropertySerializer::Save(int sceneType) {
	const std::string filePath = "./Project/Packages/Property/SceneManager.json";
	nlohmann::json data;
	data["sceneType"]["type"] = sceneType;
	std::ofstream outFile(filePath);
	if (outFile.fail()) {
		std::string message = "Faild open data file for write\n";
		//Log(message);
		return;
	}

	// -------------------------------------------------
		// ↓ ファイルに実際に書き込む
		// -------------------------------------------------
	outFile << std::setw(4) << data << std::endl;
	outFile.close();
}

void AOENGINE::SceneManagerPropertySerializer::Load(int& sceneType) {
	const std::string filePath = "./Project/Packages/Property/SceneManager.json";
	// 読み込み用ファイルストリーム
	std::ifstream ifs;
	// ファイルを読み込みように開く
	ifs.open(filePath);

	if (ifs.fail()) {
		std::string message = "not Exist " + filePath;
		return;
	}

	nlohmann::json root;
	// json文字列からjsonのデータ構造に展開
	ifs >> root;
	// ファイルを閉じる
	ifs.close();

	if (root.contains("sceneType")) {
		if (root["sceneType"].contains("type")) {
			sceneType = root["sceneType"]["type"];
		}
	}
}
