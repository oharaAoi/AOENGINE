#include "JsonSerializer.h"
#include <fstream>
#include <filesystem>

using namespace AOENGINE;
using json = nlohmann::json;

bool JsonSerializer::Save(const std::string& folderPath, const std::string& fileName, const json& json) {
	// フォルダ生成（存在しなければ）
	std::filesystem::create_directories(folderPath);

	const std::string filePath = folderPath + "/" + fileName + ".json";
	std::ofstream outFile(filePath);
	if (outFile.fail()) {
		std::string message = "Faild open data file for write\n";
		//Log(message);
		return false;
	}

	// -------------------------------------------------
	// ↓ ファイルに実際に書き込む
	// -------------------------------------------------
	outFile << std::setw(4) << json << std::endl;
	outFile.close();

	return true;
}

json JsonSerializer::Load(const std::string& folderPath, const std::string& fileName) {
	const std::string filePath = folderPath + "/" + fileName + ".json";
	// 読み込み用ファイルストリーム
	std::ifstream ifs;
	// ファイルを読み込みように開く
	ifs.open(filePath);

	if (ifs.fail()) {
		std::string message = "not Exist " + filePath;
		return json();
	}

	nlohmann::json root;
	// json文字列からjsonのデータ構造に展開
	ifs >> root;
	// ファイルを閉じる
	ifs.close();

	return root;
}