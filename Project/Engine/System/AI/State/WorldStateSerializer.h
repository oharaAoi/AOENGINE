#pragma once
#include <string>
#include <unordered_map>
#include <nlohmann/json.hpp>
#include "Engine/System/AI/State/WorldStateValue.h"

using json = nlohmann::json;

/// <summary>
/// WroldStateを保存するためのクラス
/// </summary>
class WorldStateSerializer {
public: // コンストラクタ

	WorldStateSerializer() = default;
	~WorldStateSerializer() = default;

public:

	static bool Save(const std::string& _filePath, const std::unordered_map<std::string, WorldStateValue>& _map);

	static bool Load(const std::string& _filePath, std::unordered_map<std::string, WorldStateValue>& _map);

};

