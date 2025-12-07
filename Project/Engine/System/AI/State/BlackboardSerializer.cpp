#include "BlackboardSerializer.h"
#include <iostream>
#include <fstream>

using namespace AI;

bool BlackboardSerializer::Save(const std::string& _filePath, const std::unordered_map<std::string, BlackboardValue>& _map) {
	// -------------------------------------------------
	// ↓ ファイルを開けるかのチェックを行う
	// -------------------------------------------------
	std::ofstream outFile(_filePath);
	if (outFile.fail()) {
		std::string message = "Faild open data file for write\n";
		assert(0);
		return false;
	}

	// -------------------------------------------------
	// ↓ jsonに登録
	// -------------------------------------------------
	// jsonを作成
	json result;

	for (auto& [key, value] : _map) {
		json item = value.to_json(value.value_);
		if (!item.empty()) {
			result[key] = item;
		}
	}

	// -------------------------------------------------
	// ↓ ファイルに実際に書き込む
	// -------------------------------------------------
	outFile << std::setw(4) << result << std::endl;
	outFile.close();

	return true;
}

bool BlackboardSerializer::Load(const std::string& _filePath, std::unordered_map<std::string, BlackboardValue>& _map) {
	std::ifstream inFile(_filePath);
	if (inFile.fail()) {
		assert(false && "Failed to open file");
		return false;
	}

	json j;
	inFile >> j;

	_map.clear();
	for (auto& [key, val] : j.items()) {
		BlackboardValue v;
		v.from_json(val, v);
		_map[key] = v;
	}

	return true;
}
