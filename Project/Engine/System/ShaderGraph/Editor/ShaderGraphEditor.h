#pragma once
#include <memory>
#include <unordered_set>
#include <vector>
#include <string>
#include "Engine/System/Manager/ImGuiManager.h"
#include "Engine/System/ShaderGraph/Node/BaseShaderGraphNode.h"
#include <functional>

/// <summary>
/// ShaderGraphを編集するクラス
/// </summary>
class ShaderGraphEditor {
public:

	struct NodeEntry {
		std::string path;
		std::function<void(const ImVec2&)> spawn;
	};

public:	// コンストラクタ

	ShaderGraphEditor() = default;
	~ShaderGraphEditor();

public:

	/// <summary>
	/// 初期化
	/// </summary>
	void Init();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

private:

	void ExecuteFrom(ImFlow::BaseNode* node, std::unordered_set<ImFlow::BaseNode*>& visited);

	/// <summary>
	/// 編集
	/// </summary>
	void Edit();

	/// <summary>
	/// Nodeの作成
	/// </summary>
	void CreateNode();

	/// <summary>
	/// propertyの作成
	/// </summary>
	void CreateProperty();

	/// <summary>
	/// TextureNodeの作成
	/// </summary>
	void CreteTexture();

	/// <summary>
	/// 合成Nodeの作成
	/// </summary>
	void CreateMerge();

	template<class T>
	void RegisterNode(const std::string& menuPath) {
		nodeEntries_.push_back({
			menuPath,
			[this, menuPath](const ImVec2& pos) {
				auto node = editor.addNode<T>(pos);

				// 全ノードで Init を必ず呼ぶ
				node->Init();

				// setTitle: "Category/Name" → "Name"
				auto name = menuPath.substr(menuPath.find_last_of('/') + 1);
				node->setTitle(name.c_str());
			}
							  });
	}

private:

	ImFlow::ImNodeFlow editor;

	std::vector<NodeEntry> nodeEntries_;

};

