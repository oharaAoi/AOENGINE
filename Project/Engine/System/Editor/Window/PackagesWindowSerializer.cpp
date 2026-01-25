#include "PackagesWindowSerializer.h"
#include <fstream>

void AOENGINE::PackagesWindowSerializer::Save(const std::unordered_map<std::string, bool>& stateMap, const std::string& currentPath) {
	const std::string filePath = "./Project/Packages/Property/PackagesWindow.json";
	nlohmann::json data;
	for (auto& [path, open] : stateMap) {
		data["openDir"][path] = open;
	}
	data["currentPath"]["path"] = currentPath;
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

void AOENGINE::PackagesWindowSerializer::Load(std::unordered_map<std::string, bool>& map, std::string& currentPath) {
	const std::string filePath = "./Project/Packages/Property/PackagesWindow.json";
	// 読み込み用ファイルストリーム
	std::ifstream ifs;
	// ファイルを読み込みように開く
	ifs.open(filePath);

	if (ifs.fail()) {
		std::string message = "not Exist " + filePath;
		return ;
	}

	nlohmann::json root;
	// json文字列からjsonのデータ構造に展開
	ifs >> root;
	// ファイルを閉じる
	ifs.close();

	nlohmann::json openFolderJson = root["openDir"];
	for (auto it = openFolderJson.begin(); it != openFolderJson.end(); ++it) {
		if (openFolderJson.contains(it.key())) {
			map[it.key()] = it.value().get<bool>();
		}
	}

	nlohmann::json currentPathJson = root["currentPath"];
	if (currentPathJson.contains("path")) {
		currentPath = currentPathJson["path"];
	}
}
