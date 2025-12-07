#pragma once
#include <string>
#include <unordered_map>
#include <nlohmann/json.hpp>
#include "Engine/System/AI/State/BlackboardValue.h"

using json = nlohmann::json;

namespace AI {

/// <summary>
/// WroldStateを保存するためのクラス
/// </summary>
class BlackboardSerializer {
public: // コンストラクタ

	BlackboardSerializer() = default;
	~BlackboardSerializer() = default;

public:

	static bool Save(const std::string& _filePath, const std::unordered_map<std::string, BlackboardValue>& _map);

	static bool Load(const std::string& _filePath, std::unordered_map<std::string, BlackboardValue>& _map);

};

}