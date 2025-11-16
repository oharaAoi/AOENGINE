#pragma once
#include <memory>
#include <string>
#include <unordered_set>
#include "Engine/System/Manager/ImGuiManager.h"
#include "Engine/System/ShaderGraph/ShaderGraphNodeFactory.h"
#include "Engine/Lib/Json/IJsonConverter.h"

/// <summary>
/// ShaderGraphクラス
/// </summary>
class ShaderGraph {
public:

	struct FilePathParam : public IJsonConverter {
		std::string path;

		FilePathParam() { SetName("shaderGraph"); }

		json ToJson(const std::string& id) const override {
			return JsonBuilder(id)
				.Add("path", path)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			fromJson(jsonData, "path", path);
		}

		void Debug_Gui() override {};
	};

public: // コンストラクタ

	ShaderGraph() = default;
	~ShaderGraph();

public:

	/// <summary>
	/// 初期化関数
	/// </summary>
	void Init();

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// 
	/// </summary>
	/// <param name="node"></param>
	/// <param name="visited"></param>
	void ExecuteFrom(ImFlow::BaseNode* node, std::unordered_set<ImFlow::BaseNode*>& visited);

private:

	ShaderGraphNodeFactory nodeFactory_;
	std::unique_ptr<ImFlow::ImNodeFlow> editor_;

	std::shared_ptr<ShaderGraphResultNode> resultNode_ = nullptr;

};

