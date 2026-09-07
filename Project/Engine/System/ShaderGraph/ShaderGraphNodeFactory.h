#pragma once
#include <vector>
#include <string>
#include <nlohmann/json.hpp>
#include "Engine/System/Manager/ImGuiManager.h"
#include "Engine/System/ShaderGraph/Node/BaseShaderGraphNode.h"
#include "Engine/System/ShaderGraph/Node/ShaderGraphResultNode.h"
#include <functional>
#include <memory>
#include <unordered_map>

using json = nlohmann::json;

namespace AOENGINE {

/// <summary>
/// ShaderGraphのNodeを作成する
/// </summary>
class ShaderGraphNodeFactory {
public:

	/// <summary>
	/// ShaderGraphノード登録情報
	/// </summary>
	struct NodeEntry {
		std::string path;
		std::function<std::shared_ptr<ImFlow::BaseNode>(const ImVec2&)> spawn;
		std::function<std::shared_ptr<ImFlow::BaseNode>()> spawnRuntime;
	};

public: // コンストラクタ

	ShaderGraphNodeFactory() = default;
	~ShaderGraphNodeFactory() = default;

public:

	std::shared_ptr<ShaderGraphResultNode> Init(ImFlow::ImNodeFlow* _editor);
	void InitRuntime();

	std::shared_ptr<ShaderGraphResultNode> CreateResultNode(ImFlow::ImNodeFlow* _editor);

	std::shared_ptr<ShaderGraphResultNode> CreateGraph(const json& _json);
	std::unordered_map<uintptr_t, std::shared_ptr<ImFlow::BaseNode>> CreateRuntimeGraph(
		const json& _json, std::shared_ptr<ShaderGraphResultNode>& result);

	void CreateGui(const ImVec2& _pos);

	template<class T>
	void RegisterNode(const std::string& menuPath, ImFlow::ImNodeFlow* _editor) {
		nodeEntries_.push_back({
		menuPath,
		[this, menuPath, _editor](const ImVec2& pos) ->std::shared_ptr<ImFlow::BaseNode> {
			auto node = _editor->addNode<T>(pos);
			node->Init();
			auto name = menuPath.substr(menuPath.find_last_of('/') + 1);
			node->setTitle(name.c_str());
			node->setPos(pos);
			return node;
		},
		[menuPath]() ->std::shared_ptr<ImFlow::BaseNode> {
			auto node = std::make_shared<T>();
			node->Init();
			auto name = menuPath.substr(menuPath.find_last_of('/') + 1);
			node->setTitle(name.c_str());
			return node;
		}
							   });
	}

private:

	std::vector<NodeEntry> nodeEntries_;

};

}
