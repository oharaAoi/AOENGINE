#pragma once
#include <memory>
#include <unordered_set>
#include <string>
#include "Engine/System/Manager/ImGuiManager.h"
#include "Engine/System/ShaderGraph/ShaderGraphNodeFactory.h"
#include "Engine/System/ShaderGraph/Node/ShaderGraphResultNode.h"
#include "Engine/Module/Components/Attribute/IEditorWindow.h"

namespace AOENGINE {

/// <summary>
/// ShaderGraphを編集するクラス
/// </summary>
class ShaderGraphEditor :
	public IEditorWindow {
public:	// コンストラクタ

	ShaderGraphEditor();
	~ShaderGraphEditor() override;

public:

	/// <summary>
	/// 初期化
	/// </summary>
	void Init();

	void InspectorWindow() override;

	void HierarchyWindow() override;

	void ExecutionWindow() override;

private:

	/// <summary>
	/// Nodeの更新を実行する
	/// </summary>
	/// <param name="node">: 現在のNodeのポインタ</param>
	/// <param name="visited">: 訪れたNodeのポインタマップ</param>
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
	/// Nodeを保存する
	/// </summary>
	void SaveGraph();

	/// <summary>
	/// Graphを読み込む
	/// </summary>
	void LoadGraph();

	/// <summary>
	/// Graphを上書き保存する
	/// </summary>
	void OverwriteGraph();

	/// <summary>
	/// 再読み込みする
	/// </summary>
	void Reload();

	void SaveLastPath();

	void LoadLastPath();

public:

	AOENGINE::DxResource* GetResource() const { return resultNode_->GetResultSource(); }

private:

	std::unique_ptr<ImFlow::ImNodeFlow> editor_;

	ShaderGraphNodeFactory nodeFactory_;
	std::shared_ptr<ShaderGraphResultNode> resultNode_ = nullptr;

	// 編集のための変数
	ImVec2 popupPos_;
	bool popupRequested_ = false;

	std::string graphPath_ = "";

	const std::string kPropertyPath_ = "./Project/Packages/Property/ShaderGraphEditor.json";
};

}