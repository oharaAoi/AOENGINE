#pragma once
#include <string>
#include <nlohmann/json.hpp>
#include "Engine/System/Manager/ImGuiManager.h"

using json = nlohmann::json;
namespace AOENGINE {

/// <summary>
/// ShaderGraphを外部保存する用のクラス
/// </summary>
class ShaderGraphSerializer {
public: // コンストラクタ

	ShaderGraphSerializer() = default;
	~ShaderGraphSerializer() = default;

public:

	static bool Save(const std::string& _filePath, ImFlow::ImNodeFlow* _editor);

	static json Load(const std::string& _filePath);

};
}
