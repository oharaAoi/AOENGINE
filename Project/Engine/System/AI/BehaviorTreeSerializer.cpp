#include "BehaviorTreeSerializer.h"
#include <iostream>
#include <fstream>
#include "Engine/Utilities/Logger.h"

using namespace AI;

void BehaviorTreeSerializer::Save(const std::string& filePath, const json& _json) {
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
	outFile << std::setw(4) << _json << std::endl;
	outFile.close();
}

json BehaviorTreeSerializer::LoadToJson(const std::string& filePath) {
	AOENGINE::Logger::Log("[Create][BehaviorTree] : " + filePath);
	std::ifstream inFile(filePath);
	json j;
	inFile >> j;
	AOENGINE::Logger::Log("--- success!");
	return j;
}
