#pragma once
#include <vector>
#include <string>
#include <nlohmann/json.hpp>
#include "Engine/System/Manager/ImGuiManager.h"
#include "Engine/System/ShaderGraph/Node/BaseShaderGraphNode.h"
#include <functional>
#include <memory>

using json = nlohmann::json;

/// <summary>
/// ShaderGraphのNodeを作成する
/// </summary>
class ShaderGraphNodeFactory {
public:

	struct NodeEntry {
		std::string path;
		std::function<std::shared_ptr<ImFlow::BaseNode>(const ImVec2&)> spawn;
	};

public: // コンストラクタ

	ShaderGraphNodeFactory() = default;
	~ShaderGraphNodeFactory() = default;

public:

	void Init(ImFlow::ImNodeFlow* _editor);

	void CreateGraph(const json& _json);

	void CreateGui();

	template<class T>
	void RegisterNode(const std::string& menuPath, ImFlow::ImNodeFlow* _editor) {
		nodeEntries_.push_back({
		menuPath,
		[this, menuPath, _editor](const ImVec2& pos) ->std::shared_ptr<ImFlow::BaseNode> {
			auto node = _editor->addNode<T>(pos);
			node->Init();
			auto name = menuPath.substr(menuPath.find_last_of('/') + 1);
			node->setTitle(name.c_str());
			return node; // ✅ 生成したノードを返す
		}
							   });
	}

private:

	std::vector<NodeEntry> nodeEntries_;

};

