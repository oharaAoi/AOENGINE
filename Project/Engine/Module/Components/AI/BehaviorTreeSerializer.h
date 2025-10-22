#pragma once
#include "Engine/Lib/Json/IJsonConverter.h"

/// <summary>
/// BehaviorTreeの保存/読み込み
/// </summary>
class BehaviorTreeSerializer {
public:

	// 保存
	static void Save(const std::string& filePath, const json& _json);

	// 読み込み
	static json LoadToJson(const std::string& filePath);

};

